// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Asks before a program that came from elsewhere runs for the first time: who signed it, and Open or Cancel.  Open
// notes the program and becomes the loader.

#include "signature.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <unistd.h>

#include <openssl/cms.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "base/application_bundle.h"
#include "base/file.h"
#include "base/paths.h"
#include "base/property_list.h"
#include "base/settings.h"
#include "ui/application.h"
#include "ui/cell_layout.h"
#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/dialog_window.h"
#include "ui/icon.h"
#include "ui/keys.h"
#include "ui/label.h"
#include "ui/window.h"

using namespace cutefish;

namespace {

// SHA-256 of Apple Root CA and Apple Root CA - G3.
constexpr unsigned char kAppleRoot[32] = {
    0xb0, 0xb1, 0x73, 0x0e, 0xcb, 0xc7, 0xff, 0x45, 0x05, 0x14, 0x2c, 0x49, 0xf1, 0x29, 0x5e, 0x6e,
    0xda, 0x6b, 0xca, 0xed, 0x7e, 0x2c, 0x68, 0xc5, 0xbe, 0x91, 0xb5, 0xa1, 0x10, 0x01, 0xf0, 0x24,
};
constexpr unsigned char kAppleRootG3[32] = {
    0x63, 0x34, 0x3a, 0xbf, 0xb8, 0x9a, 0x6a, 0x03, 0xeb, 0xb5, 0x7e, 0x9b, 0x3f, 0x5f, 0xa7, 0xbe,
    0x7c, 0x4f, 0x5c, 0x75, 0x6f, 0x30, 0x17, 0xb3, 0xa8, 0xc4, 0x88, 0xc3, 0x65, 0x3e, 0x91, 0x79,
};

struct Free
{
    void operator()(CMS_ContentInfo* cms) const { CMS_ContentInfo_free(cms); }
    void operator()(X509_STORE* store) const { X509_STORE_free(store); }
    void operator()(BIO* bio) const { BIO_free(bio); }
};

std::string subjectText(X509* certificate, int nid)
{
    char text[256];
    const int length = X509_NAME_get_text_by_NID(X509_get_subject_name(certificate), nid, text, sizeof(text));
    return length > 0 ? std::string(text, size_t(length)) : std::string();
}

bool isAppleRoot(X509* certificate)
{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int size = 0;
    if (!X509_digest(certificate, EVP_sha256(), digest, &size) || size != sizeof(kAppleRoot))
        return false;
    return memcmp(digest, kAppleRoot, sizeof(kAppleRoot)) == 0 || memcmp(digest, kAppleRootG3, sizeof(kAppleRootG3)) == 0;
}

// Who the signer is, as the dialog says it; empty when Apple does not vouch for the signature.
std::string developer(const std::optional<reef::Signature>& signature)
{
    if (!signature || signature->adhoc || signature->cms.empty() || signature->team.empty())
        return {};
    const unsigned char* data = reinterpret_cast<const unsigned char*>(signature->cms.data());
    std::unique_ptr<CMS_ContentInfo, Free> cms(d2i_CMS_ContentInfo(nullptr, &data, long(signature->cms.size())));
    if (!cms)
        return {};

    std::unique_ptr<X509_STORE, Free> store(X509_STORE_new());
    STACK_OF(X509)* certificates = CMS_get1_certs(cms.get());
    bool trusted = false;
    for (int i = 0; i < sk_X509_num(certificates); i++) {
        X509* certificate = sk_X509_value(certificates, i);
        if (isAppleRoot(certificate))
            trusted |= X509_STORE_add_cert(store.get(), certificate) == 1;
    }
    sk_X509_pop_free(certificates, X509_free);
    if (!trusted)
        return {};
    // The certificates of an application outlive it; Apple's extensions are unknown to OpenSSL.
    X509_STORE_set_flags(store.get(), X509_V_FLAG_NO_CHECK_TIME | X509_V_FLAG_IGNORE_CRITICAL);
    X509_STORE_set_purpose(store.get(), X509_PURPOSE_ANY);
    std::unique_ptr<BIO, Free> content(BIO_new_mem_buf(signature->codeDirectory.data(), int(signature->codeDirectory.size())));
    if (CMS_verify(cms.get(), nullptr, store.get(), content.get(), nullptr, CMS_BINARY) != 1)
        return {};

    STACK_OF(X509)* signers = CMS_get0_signers(cms.get());
    X509* signer = sk_X509_num(signers) > 0 ? sk_X509_value(signers, 0) : nullptr;
    std::string name;
    if (signer) {
        if (subjectText(signer, NID_organizationalUnitName) == signature->team)
            name = "Developer: " + subjectText(signer, NID_organizationName) + " (" + signature->team + ")";
        else if (subjectText(signer, NID_commonName) == "Apple Mac OS Application Signing")
            name = "From the Mac App Store (" + signature->team + ")";
    }
    sk_X509_free(signers);
    return name;
}

// The window moves by its content.
class Content : public ui::ColumnLayout
{
public:
    bool acceptsPointer() const override { return true; }

protected:
    void pointerPress(const ui::PointerEvent& event) override
    {
        m_pressed = event.button == ui::MouseButton::Left;
        m_position = event.position;
    }

    void pointerMove(const ui::PointerEvent& event) override
    {
        if (!m_pressed || !window())
            return;
        if (std::abs(event.position.x - m_position.x) > 10 || std::abs(event.position.y - m_position.y) > 10) {
            m_pressed = false;
            window()->startMove();
        }
    }

    void pointerRelease(const ui::PointerEvent&) override { m_pressed = false; }

private:
    bool m_pressed = false;
    ui::PointF m_position;
};

} // namespace

int main(int argc, char** argv)
{
    if (argc < 5) {
        fprintf(stderr, "Usage: reef-open <identifier> <executable> <loader> <arguments...>\n");
        return 2;
    }
    const std::string identifier = argv[1];
    const std::string executable = argv[2];
    const std::string loader = argv[3];
    const std::optional<reef::Signature> signature = reef::readSignature(executable);
    const std::string team = signature && !signature->adhoc ? signature->team : std::string();
    const std::string signer = developer(signature);

    std::string name = std::filesystem::path(executable).filename().string();
    std::string icon = "application-x-executable";
    std::string version;
    const std::string bundle = base::bundle::bundleOfProgram(executable);
    const std::optional<base::DesktopEntry> entry =
        bundle.empty() ? std::nullopt : base::bundle::read(bundle, base::DesktopEntry{});
    if (entry) {
        name = entry->name;
        if (!entry->icon.empty())
            icon = entry->icon;
    }
    const std::optional<std::string> info =
        bundle.empty() ? std::nullopt : base::file::read(bundle + "/Contents/Info.plist");
    if (info) {
        if (const auto strings = base::plist::rootStrings(*info)) {
            for (const char* key : {"CFBundleShortVersionString", "CFBundleVersion"}) {
                const auto found = strings->find(key);
                if (found != strings->end() && !found->second.empty()) {
                    version = found->second;
                    break;
                }
            }
        }
    }

    ui::Application app(argc, argv);
    app.setApplicationId("cutefish-reef");
    const std::string title = "Open “" + name + "”?";
    const std::string detail = signer.empty() ? "The developer cannot be verified." : signer;

    ui::Window window;
    window.setTitle(title);
    window.setSkipTaskbar(true);
    window.setHintedTextLayout(false);
    window.setBackgroundColor(ui::ColorId::SecondBackground);
    const auto cancel = [&app] { app.quit(); };
    window.onCloseRequested = cancel;
    window.addShortcut(0, ui::key::Escape, cancel);

    auto content = std::make_unique<Content>();
    content->setPadding(ui::Margins::all(16));
    content->setSpacing(16);
    auto* row = content->addChild<ui::RowLayout>();
    row->setSpacing(14);
    row->setLayoutHints({.fillWidth = true});
    auto* image = row->addChild<ui::ThemeIcon>(icon, 48);
    image->setLayoutHints({.preferredWidth = 48, .preferredHeight = 48, .verticalAlignment = ui::Alignment::Start});
    auto* texts = row->addChild<ui::ColumnLayout>();
    texts->setSpacing(4);
    texts->setLayoutHints({.fillWidth = true, .verticalAlignment = ui::Alignment::Start});
    auto* titleLabel = texts->addChild<ui::Label>(title);
    titleLabel->setLayoutHints({.fillWidth = true});
    titleLabel->setWrap(true);
    titleLabel->setFontSize(14);
    titleLabel->setWeight(600);
    auto* detailLabel = texts->addChild<ui::Label>(detail);
    detailLabel->setLayoutHints({.fillWidth = true});
    detailLabel->setWrap(true);
    detailLabel->setFontSize(12);
    detailLabel->setColor(ui::ColorId::DisabledText);
    if (!version.empty()) {
        auto* versionLabel = texts->addChild<ui::Label>("Version " + version);
        versionLabel->setLayoutHints({.fillWidth = true});
        versionLabel->setWrap(true);
        versionLabel->setFontSize(12);
        versionLabel->setColor(ui::ColorId::DisabledText);
    }

    auto* buttons = content->addChild<ui::RowLayout>();
    buttons->setSpacing(8);
    buttons->setLayoutHints({.fillWidth = true});
    buttons->addChild<ui::View>()->setLayoutHints({.fillWidth = true});
    auto* cancelButton = buttons->addChild<ui::DialogButton>("Cancel");
    cancelButton->onClicked = cancel;
    auto* openButton = buttons->addChild<ui::DialogButton>("Open", true);
    char** arguments = argv + 4;
    const auto open = [&window, &identifier, &team, &loader, arguments] {
        {
            base::Settings settings(base::paths::cutefishConfig("reef"));
            settings.setValue("OpenedApplications/" + identifier, team);
        }
        window.requestActivationToken([&loader, arguments](const std::string& token) {
            if (!token.empty())
                setenv("XDG_ACTIVATION_TOKEN", token.c_str(), 1);
            execv(loader.c_str(), arguments);
            _exit(127);
        });
    };
    openButton->onClicked = open;
    window.addShortcut(0, ui::key::Return, open);

    window.setContentView(std::move(content));
    window.resize({420, 0});
    window.setFitsContentHeight(true);
    window.show();
    window.activate();
    openButton->setFocus(ui::FocusReason::Other);
    return app.run();
}
