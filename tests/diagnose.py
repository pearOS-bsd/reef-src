#!/usr/bin/env python3
"""Differential test of reef-diagnose: the reports of the original and of ours on the same made up machines (a library that
gives them files of a folder for /sys, /proc and /etc; programs that print what the test wants).  Times are masked.
usage: diagnose.py ORIGINAL OURS"""
import os, re, shutil, subprocess, sys, tempfile

original, ours = sys.argv[1], sys.argv[2]
work = tempfile.mkdtemp()
LIBRARY = r'''
#define _GNU_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
static const char* const places[] = {"/sys/class/drm", "/sys/module/i915/", "/sys/module/xe/", "/etc/os-release", "/etc/debian_version",
                                     "/proc/cpuinfo", "/proc/meminfo", "/proc/cmdline"};
static const char* place(const char* path, char* buffer)
{
    const char* root = getenv("FAKE_ROOT");
    if (root && path)
        for (int i = 0; i < 8; i++)
            if (strncmp(path, places[i], strlen(places[i])) == 0) {
                snprintf(buffer, 4096, "%s%s", root, path);
                return buffer;
            }
    return path;
}
FILE* fopen(const char* path, const char* mode)
{
    char buffer[4096];
    return ((FILE* (*)(const char*, const char*))dlsym(RTLD_NEXT, "fopen"))(place(path, buffer), mode);
}
DIR* opendir(const char* path)
{
    char buffer[4096];
    return ((DIR* (*)(const char*))dlsym(RTLD_NEXT, "opendir"))(place(path, buffer));
}
ssize_t readlink(const char* path, char* out, size_t size)
{
    char buffer[4096];
    return ((ssize_t (*)(const char*, char*, size_t))dlsym(RTLD_NEXT, "readlink"))(place(path, buffer), out, size);
}
int gethostname(char* name, size_t size)
{
    const char* host = getenv("FAKE_HOST");
    snprintf(name, size, "%s", host ? host : "testhost");
    return 0;
}
struct passwd* getpwuid(uid_t uid)
{
    static struct passwd user;
    static char home[4096];
    const char* kind = getenv("FAKE_USER");
    if (kind && strcmp(kind, "none") == 0)
        return NULL;
    snprintf(home, sizeof(home), "%s/home/jdoe", getenv("FAKE_ROOT"));
    user.pw_name = "jdoe";
    user.pw_passwd = "";
    user.pw_gecos = kind && strcmp(kind, "short") == 0 ? "Jo" : "John Doe,Room 1,,";
    user.pw_dir = kind && strcmp(kind, "homeless") == 0 ? NULL : home;
    user.pw_shell = "/bin/sh";
    return &user;
}
int uname(struct utsname* names)
{
    memset(names, 0, sizeof(*names));
    strcpy(names->sysname, "Linux");
    strcpy(names->release, "9.9.9-test");
    strcpy(names->version, "#1 SMP testhost build");
    strcpy(names->machine, "x86_64");
    return 0;
}
time_t time(time_t* t) { if (t) *t = 1795622163; return 1795622163; }
'''
preload = os.path.join(work, 'fake.so')
with open(preload + '.c', 'w') as f:
    f.write(LIBRARY)
subprocess.run(['gcc', '-w', '-shared', '-fPIC', '-o', preload, preload + '.c', '-ldl'], check=True)

def write(root, path, text='', mode=0o644):
    path = os.path.join(root, path.lstrip('/'))
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(text)
    os.chmod(path, mode)

def link(root, path, target):
    path = os.path.join(root, path.lstrip('/'))
    os.makedirs(os.path.dirname(path), exist_ok=True)
    os.symlink(target, path)

def plain(root):
    write(root, '/sys/class/drm/card0-eDP-1/status', 'connected\n')
    write(root, '/sys/class/drm/card0-eDP-1/modes', '64x48\n')
    os.makedirs(os.path.join(root, 'home/jdoe'), exist_ok=True)

JOURNAL = '''#!/bin/sh
case "$*" in
"-k -b -o short-iso-precise --no-pager -q") cat "$FAKE_ROOT/iso";;
"-k -b -o short-monotonic --no-pager -q")
    echo x >> "$FAKE_ROOT/calls"
    cat "$FAKE_ROOT/monotonic"
    if [ $(wc -l < "$FAKE_ROOT/calls") -ge 3 ]; then cat "$FAKE_ROOT/later"; fi;;
*) echo "journalctl $*"; exit 1;;
esac
'''

def journal(root, monotonic, iso='', later=''):
    write(root, '/programs/journalctl', JOURNAL, 0o755)
    write(root, '/monotonic', monotonic)
    write(root, '/iso', iso)
    write(root, '/later', later)

def intel(root):
    plain(root)
    write(root, '/etc/os-release', 'NAME="Cute"\nPRETTY_NAME="Cute OS 1 (test)"\nVERSION=1\nPRETTY_NAME=other\n')
    write(root, '/etc/debian_version', ' 13.1 \n\n')
    write(root, '/proc/cpuinfo', 'processor\t: 0\nmodel\t\t: 1\nmodel name\t: Test CPU: 5 @ 1GHz\nmodel name\t: second\n')
    write(root, '/proc/meminfo', 'MemTotal:       123456 kB\nMemFree: 1 kB\n')
    write(root, '/proc/cmdline', 'BOOT=x i915.enable_psr=0 quiet xe.foo=1 nomodeset amdgpu.dc=1 nvidia-drm.modeset=1 drm.debug=1 splash '
          'radeon.x nouveau.y=1 i915 xe i915x.y drm nomodesetx xe.\n')
    for name in ('card0', 'renderD128'):
        d = '/sys/class/drm/' + name
        write(root, d + '/device/uevent', 'DRIVER=i915\nPCI_CLASS=30000\nPCI_ID=8086:9A49\nPCI_SUBSYS_ID=1558:51A1\nPCI_SLOT_NAME=0000:00:02.0\n')
        link(root, d + '/device/driver', '../../../bus/pci/drivers/i915')
        write(root, d + '/device/boot_vga', '1\n')
    c = '/sys/class/drm/card0'
    write(root, c + '/gt_act_freq_mhz', '350\n')
    write(root, c + '/gt_RP0_freq_mhz', '1300\n')
    write(root, c + '/gt_max_freq_mhz', '1200\n')
    write(root, c + '/gt_min_freq_mhz', '100\n')
    write(root, c + '/engine/rcs0/preempt_timeout_ms', '100000\n')
    write(root, c + '/gt/gt0/throttle_reason_status', '1\n')
    write(root, c + '/gt/gt0/throttle_reason_pl1', '1\n')
    write(root, c + '/gt/gt0/throttle_reason_thermal', '0\n')
    write(root, c + '/gt/gt0/throttle_reason_vr_tdc', '2\n')
    write(root, '/sys/class/drm/card0-eDP-1/modes', '64x48\n' + ''.join('%dx%d\n' % (1000 - i, 600 - i) for i in range(8)))
    write(root, '/sys/class/drm/card0-HDMI-A-1/status', 'disconnected\n')
    write(root, '/sys/class/drm/card0-DP-1/status', 'connected\n')
    write(root, '/sys/class/drm/card0-DP-1/modes', '')
    write(root, '/sys/class/drm/card0-DP-2/status', ' connected\t\n')
    write(root, '/sys/class/drm/card0-DP-2/modes', 'nothing\n32x32\n')
    write(root, '/sys/class/drm/version', 'drm 1.1.0\n')
    write(root, '/sys/class/drm/.hidden', '')
    for module, parameters in (('i915', {'enable_guc': '-1', 'enable_psr': '0\n', 'force_probe': '*', 'mitigations': 'auto', 'enable_dc': ' \n'}),
                               ('xe', {'other': '1'})):
        for name, value in parameters.items():
            write(root, '/sys/module/%s/parameters/%s' % (module, name), value)
    write(root, '/programs/dpkg-query', '#!/bin/sh\necho "args $#"\necho "cutefish-reef 0.1-1 ii "\necho "libegl1 1.7 rc "\necho "libgbm1 25 un"\n'
          'echo "dpkg-query: no packages found matching x"\necho\necho "firmware-intel-graphics"\necho "libdrm2 2.4 iia more"\nexit 1\n', 0o755)
    write(root, '/programs/eglinfo', '#!/bin/sh\necho "EGL of testhost for John Doe"\necho "errors" >&2\nexit 3\n', 0o755)
    lines = []
    for i in range(340):
        lines.append('[%5d.%06d] testhost kernel: i915 0000:00:02.0: [drm] line %d of %s\n' % (i, i, i, root))
        lines.append('[%5d.%06d] testhost kernel: usb nothing %d\n' % (i, i, i))
    for text in ('Xe started', 'xen not', 'the DRM here', 'drmx', '[drmx] yes', 'AMDGPU', 'GPU Hang', 'guc', 'gucx', 'the huc', 'dmc.', 'gfx', 'ring rcs0',
                 'render ring', 'dma-buf', 'dmabuf', 'gem', 'gems', 'RESET', 'nVidia', 'jdoe xjdoe jdoe_x jdoe-x jdoe. (jdoe)', 'John Doe reset Johns'):
        lines.append('[ 1000.0] testhost kernel: %s\n' % text)
    iso = ''.join('2026-11-25T%02d:%02d:%02d.%s%s testhost kernel: %s\n' % row for row in (
        (10, 0, 10, '500000', '+0200', 'i915 0000:00:02.0: [drm] GPU HANG: ecode 12:1:85dffffb, in Safari [4242]'),
        (10, 0, 11, '250000', '+0200', 'i915 0000:00:02.0: [drm] Safari[4242] context reset due to GPU hang'),
        (10, 5, 0, '000001', '+02:00', 'i915 0000:00:02.0: [drm] Xcode[77] context reset due to GPU hang'),
        (3, 6, 0, '0', '-0500', 'amdgpu 0000:03:00.0: amdgpu: Process information: process Finder pid 900 thread Finder:cs0 pid 901'),
        (10, 7, 0, '9', '+0000', 'no hang')))
    journal(root, ''.join(lines), iso + 'Mail[5] context reset due to GPU hang without a time\n',
            '[ 2000.0] testhost kernel: GPU HANG: ecode 1\n[ 2000.1] kernel: Resetting rcs0 for preemption time out\n'
            '[ 2000.2] kernel: Resetting 1 for\n[ 2000.3] kernel: amdgpu: GPU reset begin!\n[ 2000.4] kernel: ring timeout\n')
    write(root, '/home/jdoe/.cache/cutefish-reef/gpu-activity.log.old',
          '2026-11-25T08:00:10.000000+0000 Safari pid 4242 tid 4300 SLOW Metal command buffer for 900 ms: old | also running: nothing else\n'
          'not a line\n')
    write(root, '/home/jdoe/.cache/cutefish-reef/gpu-activity.log',
          '2026-11-25T08:00:10.600000+0000 Safari pid 4242 tid 4242 RUNNING Metal command buffer for 200 ms: named pass | also running: nothing else\n'
          '2026-11-25T08:00:10.700000+0000 Safari pid 4242 tid 4242 SLOW Metal command buffer for 300 ms: longer named pass\n'
          '2026-11-25T08:00:10.800000+0000 Safari pid 4242 tid 9 SLOW Metal command buffer for 5000 ms: longest | also running: x\n'
          '2026-11-25T08:00:30.000000+0000 Safari pid 4242 tid 4242 SLOW after\n'
          '2026-11-25T08:05:00.300000+0000 X code pid 78 tid 77 SLOW Metal of %s for 100 ms: t | also running: nothing else\n'
          '2026-11-25T08:06:00.100000+0000 Finder pid 900 tid 1 SLOW blit for 120 ms: a\n'
          '2026-11-25T08:06:00.200000+0000 Finder pid 900 tid 2 SLOW draw for 120 ms: b\n'
          '2026-11-25T08:06:00.500000+0000 Finder pid 900 tid 2 SLOW draw for 900 ms: at the edge\n' % root +
          ''.join('2026-11-25T09:%02d:00.0+0000 App%d pid %d tid %d SLOW work for %d ms: many\n' % (i % 60, i, i + 10000, i + 10000, i) for i in range(230)))
    write(root, '/probe', 'RUN memory\nINFO device: made up\nPASS memory.a: good: yes\nRUN shaders\nstray output\nFAIL shaders.b: bad\nWARN shaders.c: slow\n'
          'SKIP shaders.d: none\nRUN x\nFAIL nocolon\nPASS \nDONE all\n')

def amd(root):
    plain(root)
    write(root, '/etc/os-release', "PRETTY_NAME='single'\n")
    write(root, '/proc/cpuinfo', 'model name without colon\n')
    write(root, '/proc/cmdline', 'quiet\n')
    c = '/sys/class/drm/card1'
    write(root, c + '/device/uevent', 'PCI_ID=1002:73BF\n')
    link(root, c + '/device/driver', 'amdgpu')
    write(root, c + '/device/boot_vga', '0\n')
    write(root, c + '/device/gpu_busy_percent', '37\n')
    write(root, c + '/device/pp_dpm_sclk', '0: 500Mhz\n1: 1200Mhz *\n2: 2500Mhz\n')
    c = '/sys/class/drm/card2'
    write(root, c + '/device/tile0/gt0/freq0/act_freq', '1000\n')
    write(root, c + '/gt/gt0/throttle_reason_status', '0\n')
    write(root, c + '/gt_act_freq_mhz', 'unknown\n')
    write(root, c + '/device/pp_dpm_sclk', '0: 500Mhz *')
    write(root, '/sys/class/drm/card3/device/pp_dpm_sclk', 'none marked\n')
    write(root, '/sys/class/drm/card4/device/pp_dpm_sclk', 'marked * without a number\n')
    write(root, '/sys/class/drm/card4/gt_max_freq_mhz', '12abc\n')
    write(root, '/sys/class/drm/card4/gt_min_freq_mhz', '-5\n')
    write(root, '/sys/class/drm/card0-eDP-1/modes', '0x0\n64x48\n')
    write(root, '/sys/class/drm/card1-DP-9/status', 'connected\n')
    write(root, '/sys/class/drm/card1-DP-9/modes', '48x40 60Hz\n')
    journal(root, '[ 1.0] nothing of interest\n', '', '')
    write(root, '/programs/dpkg-query', '#!/bin/sh\nexit 2\n', 0o755)
    write(root, '/probe', 'RUN crash\nhalf\n')
    write(root, '/probe-status', '139')

def huge(root):
    plain(root)
    journal(root, '', '', '')
    write(root, '/sys/class/drm/card4/gt_RP0_freq_mhz', '99999999999999999999999\n')

def bare(root):
    plain(root)
    journal(root, '', '', '')
    write(root, '/probe', 'RUN first\nPASS first.a: x\nno more')

def failing(root):
    plain(root)
    write(root, '/programs/journalctl', '#!/bin/sh\necho "no journal"\nexit 1\n', 0o755)
    write(root, '/probe', 'DONE \n')

folders = {}
for build in (plain, intel, amd, huge, bare, failing):
    folders[build.__name__] = build

failed = 0
total = 0

def mask(text, where):
    text = text.replace(where, 'PROGRAMS')
    text = re.sub(r'(?m)^(  )?(PASS|WARN|FAIL) (glass\.[a-z-]+): (\d+ panes, \d+ frames): .*$', r'\1glass \3: \4', text)
    text = re.sub(r'(?m)^  glass .*\n', '', text)
    text = re.sub(r'over \d+ samples', 'over N samples', text)
    text = re.sub(r'\d+ of \d+ samples', 'N of N samples', text)
    text = re.sub(r'(?m)^Summary: .*$', 'Summary', text)
    text = re.sub(r'(?m)^\d+ failed, \d+ warnings\.$', 'counts', text)
    return text

def counts(text):
    """The numbers at the start of a report are those of its lines."""
    match = re.search(r'(?m)^Summary: (\d+) failed, (\d+) warnings$', text)
    body = text[text.index('\n== '):] if '\n== ' in text else ''
    return match and (int(match[1]), int(match[2])) == (len(re.findall(r'(?m)^FAIL [^:\n]*: ', body)), len(re.findall(r'(?m)^WARN [^:\n]*: ', body)))

def compare(name, build=None, arguments=('REPORT',), probe=True, environment=(), output='out/report.txt', programs=True):
    global failed, total
    total += 1
    results = []
    for program, side in ((original, 'a'), (ours, 'b')):
        root = os.path.join(work, 'machine')
        shutil.rmtree(root, ignore_errors=True)
        os.makedirs(os.path.join(root, 'out'))
        os.makedirs(os.path.join(root, 'programs'))
        if build:
            folders[build](root)
        where = os.path.join(work, 'side')
        shutil.rmtree(where, ignore_errors=True)
        os.makedirs(os.path.join(where, 'bin'))
        shutil.copy(program, os.path.join(where, 'bin', 'reef-diagnose'))
        if probe:
            write(where, 'bin/reef', '#!/bin/sh\necho "  args $# $2 $3" | sed "s/^/INFO args: /"\ncat "$FAKE_ROOT/probe"\n'
                  'exit $(cat "$FAKE_ROOT/probe-status" 2>/dev/null || echo 0)\n', 0o755)
            write(where, 'lib/cutefish-reef/usr/libexec/reef-gpu-probe', 'x')
        paths = os.path.join(root, 'programs') + (':/usr/bin:/bin' if programs else '')
        env = dict(os.environ, LD_PRELOAD=preload, FAKE_ROOT=root, PATH=paths, TZ='UTC', **dict(environment))
        command = [os.path.join(where, 'bin', 'reef-diagnose')] + [a.replace('REPORT', os.path.join(root, 'out/report.txt')) for a in arguments]
        result = subprocess.run(command, cwd=os.path.join(root, 'out'), env=env, capture_output=True, timeout=600)
        report = os.path.join(root, output)
        text = open(report, errors='replace').read() if os.path.exists(report) else None
        if text is not None and not counts(text):
            print('%s: the counts of %s are not those of its lines' % (name, program))
            failed += 1
        results.append((result.returncode, mask(result.stdout.decode(errors='replace'), where), mask(result.stderr.decode(errors='replace'), where),
                        text if text is None else mask(text, where)))
    if results[0] == results[1]:
        print('%s: same, exit %d, report of %s' % (name, results[0][0], len(results[0][3]) if results[0][3] else None))
        return
    failed += 1
    print('%s: differs' % name)
    for a, b in zip(results[0], results[1]):
        if a != b:
            a, b = str(a).splitlines(), str(b).splitlines()
            import difflib
            print('\n'.join(list(difflib.unified_diff(a, b, 'original', 'ours', lineterm='', n=1))[:60]))

compare('plain', 'plain')
compare('intel', 'intel')
compare('amd', 'amd')
compare('huge number', 'huge', probe=False)
compare('bare', 'bare')
compare('failing', 'failing')
compare('no probe', 'plain', probe=False)
compare('no programs', 'plain', probe=False, programs=False)
compare('default place', 'plain', arguments=(), output='home/jdoe/reef-diagnose.txt', probe=False)
compare('no user', 'plain', arguments=(), output='out/reef-diagnose.txt', environment={'FAKE_USER': 'none'}, probe=False)
compare('no home', 'plain', arguments=(), output='out/reef-diagnose.txt', environment={'FAKE_USER': 'homeless'}, probe=False)
compare('short names', 'intel', environment={'FAKE_USER': 'short', 'FAKE_HOST': 'ab'})
compare('not writable', 'plain', arguments=('/nonexistent/report',), probe=False)
compare('usage', arguments=('a', 'b'))
compare('option', arguments=('--help',))
compare('import few', arguments=('--import-check', '1', '2'))
compare('import planes 0', arguments=('--import-check', '1', '2', '3', '4', '5', '0', '1', '2', '3'))
compare('import planes 5', arguments=('--import-check', '1', '2', '3', '4', '5', '5') + ('1',) * 15)
compare('import planes few', arguments=('--import-check', '1', '2', '3', '4', '5', '2', '1', '2', '3'))
compare('import bad', arguments=('--import-check', '256', '256', '875708993', '0', '7', '1', '0', '0', '1024'))
shutil.rmtree(work)
print('%d cases, %d differ' % (total, failed))
sys.exit(1 if failed else 0)
