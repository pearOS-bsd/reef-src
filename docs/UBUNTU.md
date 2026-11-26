# open-reef on Ubuntu 24.04 (KDE neon)

The packages are built for Debian trixie. On Ubuntu 24.04 they run after four changes (checked on KDE neon, Plasma Wayland:
NetNewsWire, CotEditor, Ghostty, Keka and MyGo Music start and work):

1. **Dependency versions.** cutefish-toolkit, cutefish-webkit and the reef package declare trixie's versions (libpng 1.6.46,
   libwebp 1.5, GStreamer 1.25, libavif 1.2, libclang-cpp19 19.1.7); every symbol they use exists in Ubuntu's older versions.
   `tools/ubuntu/relax.py OUT_DIR PACKAGE.deb...` rewrites the packages without the constraints the system cannot meet (version
   `+local1`). libicu76 and libjpeg62-turbo install from trixie's packages as they are.
2. **Toolkit version.** The reef package was built against cutefish-toolkit 7e25ddc. The later 111537f removed
   `ui::View::setGlass(std::vector<ui::ViewGlass>)`, so the AppKit host does not load with it ("cannot load the AppKit host
   library"). Install the 7e25ddc libraries.
3. **GStreamer 1.26.** cutefish-webkit calls `gst_structure_foreach_id_str`, `gst_structure_filter_and_map_in_place_id_str` and
   `gst_id_str_as_str` (1.26). The loader opens host libraries with `RTLD_NOW`, so the WebKit host would not load.
   `tools/ubuntu/gstcompat.c` implements them on 1.24's quark API; it is added to libcutefish-webkit.so.2 with
   `patchelf --add-needed libcutefish-webkit-gstcompat.so.0`.
4. **User namespaces.** Ubuntu restricts unprivileged user namespaces (`kernel.apparmor_restrict_unprivileged_userns = 1`), so
   WebKit's bubblewrap sandbox fails ("bwrap: setting up uid map: Permission denied", then a crash in dbus-proxy).
   `tools/ubuntu/reef.apparmor` in /etc/apparmor.d/reef gives reef and what it starts the `userns` permission, as Ubuntu's own
   profile does for epiphany.
