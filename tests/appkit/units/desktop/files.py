# What the scenarios work with: a home with files of several types, desktop entries, lists of defaults.
import os, shutil, stat

types = ['~', '', 'text/plain', 'application/pdf', 'application/x-pdf', 'image/png', 'inode/directory', 'x-scheme-handler/http', 'text/x-csrc', 'video/mp4',
         'application/x-zerosize', 'no/such-type', 'nonsense', 'a/b/c', '/', 'application/octet-stream', 'audio/mpeg', 'TEXT/PLAIN', 'application/zip']

identifiers = ['com.example.App', 'com.apple.TextEdit', 'single', '3dapp.x.9y', 'a..b', '.a', 'a.', '.', '...', 'foo-bar_baz.qux', 'ünï.cøde', 'with space.x y', '9',
               '0.1.2', 'A.B.C', '-', '_', 'x.' + 'y' * 60, 'x.' + 'y' * 300, 'a\\b', 'com.example.app:1', '/path/x', 'a.0', 'com.Example_1-2.3D']

def write(path, data, mode=None):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, 'wb').write(data if isinstance(data, bytes) else data.encode())
    if mode: os.chmod(path, mode)

def entry(**keys):
    return '[Desktop Entry]\n' + ''.join('%s=%s\n' % (k.replace('__', '-'), v) for k, v in keys.items())

def make(home):
    shutil.rmtree(home, ignore_errors=True)
    for d in ('run', '.config', '.local/share', 'bin'): os.makedirs(home + '/' + d)
    os.chmod(home + '/run', 0o700)
    shutil.copytree(os.path.dirname(os.path.abspath(__file__)) + '/mime', home + '/.local/share/mime')
    write(home + '/f/text.txt', 'hello\n'); write(home + '/f/empty', ''); write(home + '/f/noext', 'plain text\n'); write(home + '/f/bin', bytes(range(256)) * 4)
    write(home + '/f/a.png', b'\x89PNG\r\n\x1a\n' + bytes(40)); write(home + '/f/x.pdf', b'%PDF-1.4\n'); write(home + '/f/pdfnoext', b'%PDF-1.4\n%\xe2\n')
    write(home + '/f/ünï.c', 'int main;\n'); write(home + '/f/with space.html', '<html></html>'); write(home + '/f/z.zip', b'PK\x03\x04' + bytes(30))
    write(home + '/f/secret.txt', 'x'); os.chmod(home + '/f/secret.txt', 0)
    os.symlink('text.txt', home + '/f/link'); os.symlink('nothing', home + '/f/broken.txt'); os.mkfifo(home + '/f/fifo.txt') if False else None
    # what a started program was given
    write(home + '/bin/note', '#!/bin/sh\n{ echo "$0"; for a in "$@"; do echo "[$a]"; done; pwd; } > %s/started.tmp\nmv %s/started.tmp %s/started\n' % (home, home, home), 0o755)
    write(home + '/bin/notexecutable', '#!/bin/sh\n', 0o644)
    a = home + '/.local/share/applications/'
    write(a + 'one.desktop', entry(Type='Application', Name='One', Exec='note --one %F end', MimeType='text/plain;application/pdf;', Icon='one-icon'))
    write(a + 'two.desktop', entry(Type='Application', Name='Two Words', Exec='note "quoted arg" %U %i %c %k', Icon='two', MimeType='text/plain;x-scheme-handler/http;'))
    write(a + 'single.desktop', entry(Type='Application', Name='Single', Exec='note %f', MimeType='image/png;'))
    write(a + 'url.desktop', entry(Type='Application', Name='Url', Exec='note --url=%u %%', MimeType='x-scheme-handler/http;'))
    write(a + 'none.desktop', entry(Type='Application', Name='None', Exec='note fixed', MimeType='video/mp4;'))
    write(a + 'noexec.desktop', entry(Type='Application', Name='No exec'))
    write(a + 'emptyexec.desktop', entry(Type='Application', Name='Empty', Exec=''))
    write(a + 'codes.desktop', entry(Type='Application', Name='Codes', Exec='%f'))
    write(a + 'missing.desktop', entry(Type='Application', Name='Missing', Exec='no-such-program-at-all %f'))
    write(a + 'notexec.desktop', entry(Type='Application', Name='Not executable', Exec=home + '/bin/notexecutable'))
    write(a + 'link.desktop', entry(Type='Link', Name='Link', URL='http://x/'))
    write(a + 'noname.desktop', entry(Type='Application', Exec='note'))
    write(a + 'hidden.desktop', entry(Type='Application', Name='Hidden', Exec='note hidden', Hidden='true'))
    write(a + 'nodisplay.desktop', entry(Type='Application', Name='No display', Exec='note nodisplay %F', NoDisplay='true', MimeType='audio/mpeg;'))
    write(a + 'path.desktop', entry(Type='Application', Name='Path', Exec='note in path', Path=home + '/f'))
    write(a + 'terminal.desktop', entry(Type='Application', Name='Terminal', Exec='note terminal', Terminal='true'))
    write(a + 'try.desktop', entry(Type='Application', Name='Try', Exec='note try', TryExec='no-such-program'))
    write(a + 'action.desktop', entry(Type='Application', Name='Action', Exec='note main %F', Actions='New;') + '\n[Desktop Action New]\nName=New\nExec=note new\n')
    write(a + 'localized.desktop', entry(Type='Application', Name='Plain', **{'Name[ro]': 'Românește'}, Exec='note %c'))
    write(a + 'garbage.desktop', bytes(range(256)) * 20)
    write(a + 'empty.desktop', '')
    write(a + 'long.desktop', entry(Type='Application', Name='Long', Exec='note ' + ' '.join('argument-number-%d' % i for i in range(60)) + ' %F', Comment='c' * 5000))
    write(a + 'crlf.desktop', entry(Type='Application', Name='Crlf', Exec='note crlf %F').replace('\n', '\r\n'))
    write(a + 'absolute.desktop', entry(Type='Application', Name='Absolute', Exec=home + '/bin/note absolute'))
    write(home + '/local.desktop', entry(Type='Application', Name='Local', Exec='note local %k %F'))
    write(home + '/noextension', entry(Type='Application', Name='No extension', Exec='note no extension %F'))
    write(home + '/.config/mimeapps.list', '[Default Applications]\ntext/plain=two.desktop\nimage/png=gone.desktop;single.desktop\nx-scheme-handler/http=url.desktop;\naudio/mpeg=nodisplay.desktop\n\n[Added Associations]\nvideo/mp4=one.desktop;none.desktop;\n\n[Removed Associations]\napplication/pdf=one.desktop;\n')
    write(home + '/.local/share/applications/mimeapps.list', '[Default Applications]\napplication/zip=one.desktop\napplication/pdf=one.desktop;two.desktop\ninode/directory=single.desktop\n')

def paths(home):
    f = home + '/f/'
    return ['~', '', home, f, f + 'text.txt', f + 'empty', f + 'noext', f + 'bin', f + 'a.png', f + 'x.pdf', f + 'pdfnoext', f + 'ünï.c', f + 'with space.html', f + 'z.zip',
            f + 'secret.txt', f + 'link', f + 'broken.txt', f + 'not there.mp4', '/no/such/dir/x', 'f/text.txt', 'relative.odt', '/dev/null', '/proc/self/cmdline', '/bin/sh',
            home + '/.local/share/applications/one.desktop', home + '/bin/note']

def launches(home):
    a = home + '/.local/share/applications/'
    argsets = [[], ['/a'], ['/a b', 'http://x/y z', '', '/c']]
    result = []
    for n in sorted(os.listdir(a)):
        for s in argsets: result.append((a + n, s))
    for p in (home + '/local.desktop', 'local.desktop', './local.desktop', home + '/noextension', 'noextension', home + '/nothere.desktop', 'nothere', '', '/', home, home + '/',
              a + 'one.desktop/', a + '../applications/one.desktop', '//' + a + 'one.desktop'):
        for s in argsets[:2]: result.append((p, s))
    result.append((a + 'one.desktop', ['x%d' % i for i in range(30)]))
    result.append((a + 'one.desktop', ['%f', '"q"', "it's", '\\', '$HOME', 'a;b', 'y' * 3000]))
    return result
