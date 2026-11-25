# The build environment of the original binaries (inferred: Debian trixie, see docs/TOOLCHAIN.md).
FROM debian:trixie
# Fixed package versions: the archive as it was on the day of the original build.
RUN rm -f /etc/apt/sources.list.d/* && for s in "debian trixie" "debian trixie-updates" "debian-security trixie-security"; do \
      set -- $s; echo "deb [check-valid-until=no] http://snapshot.debian.org/archive/$1/20261123T120000Z $2 main" >> /etc/apt/sources.list; done
RUN apt-get -o Acquire::Retries=5 update && apt-get -o Acquire::Retries=5 install -y --allow-downgrades --no-install-recommends \
    build-essential cmake ninja-build pkg-config python3 python3-pyelftools python3-capstone git file \
    gdb strace ca-certificates gettext dpkg-dev clang lld-19 procps \
    libwayland-dev wayland-protocols libegl-dev libgles-dev libgl1-mesa-dri libxkbcommon-dev libsystemd-dev \
    libfontconfig-dev libfreetype-dev libharfbuzz-dev libicu-dev libpng-dev libjpeg-dev zlib1g-dev \
    libexpat1-dev libwebp-dev libtiff-dev libheif-dev libjxl-dev libssl-dev liblzma-dev libxml2-dev libgbm-dev libglib2.0-dev \
    && rm -rf /var/lib/apt/lists/*
