# First Yocto Build

## Summary

This note documents the first successful Yocto build and QEMU boot test.

## Environment

- Host: WSL2 Ubuntu
- Yocto branch: scarthgap
- Poky version: 5.0.19
- Machine: qemux86-64
- Image: core-image-minimal
- Build directory: ~/git/yocto/poky/build-qemux86-64
- Custom lab repository: ~/git/yocto-labs

## Local Workspace Layout

```text
~/git/yocto/poky/        Official Poky source tree
~/git/yocto/downloads/   Yocto download cache
~/git/yocto/sstate-cache/ Yocto shared state cache
~/git/yocto-labs/        Custom Yocto labs, notes, scripts, and layers
```

## Build Setup

Enter the Yocto build environment:

```bash
cd ~/git/yocto/poky
source oe-init-build-env build-qemux86-64
```

The build directory becomes:

```text
~/git/yocto/poky/build-qemux86-64
```

## Build Command

Build the minimal image:

```bash
bitbake core-image-minimal
```

## Parallel Build Settings

The build was stabilized on WSL by limiting BitBake task parallelism while still allowing make to use more jobs.

Example configuration in `conf/local.conf`:

```conf
BB_NUMBER_THREADS = "6"
BB_NUMBER_PARSE_THREADS = "6"
PARALLEL_MAKE = "-j 12"
PARALLEL_MAKEINST = "-j 12"
```

Meaning:

- `BB_NUMBER_THREADS`: maximum number of BitBake tasks running at the same time
- `PARALLEL_MAKE`: number of parallel jobs used inside make-based compile tasks

## Build Artifacts

Generated images are located under:

```bash
tmp/deploy/images/qemux86-64/
```

Useful files include:

```text
bzImage
core-image-minimal-qemux86-64.rootfs.ext4
core-image-minimal-qemux86-64.rootfs.manifest
core-image-minimal-qemux86-64.rootfs.qemuboot.conf
```

## QEMU Boot

Run the image with QEMU:

```bash
runqemu qemux86-64 nographic
```

If `runqemu` cannot automatically find the image, specify the qemuboot configuration file directly:

```bash
runqemu tmp/deploy/images/qemux86-64/core-image-minimal-qemux86-64.rootfs.qemuboot.conf nographic
```

## Basic Target Checks

After booting into QEMU, log in as:

```text
root
```

Then run:

```bash
uname -a
cat /etc/os-release
df -h
ip addr
ps
dmesg | tail
```

## QEMU Exit

From inside the target:

```bash
poweroff
```

If needed, use the QEMU escape sequence:

```text
Ctrl-A X
```

## Notes

- The first build took a long time because Yocto had to build the toolchain, kernel, root filesystem, and packages.
- Later builds should be much faster because Yocto reuses downloads, task outputs, and shared state cache.
- WSL stability was an issue with aggressive parallelism.
- `screen` can protect against terminal disconnects, but it cannot survive a full WSL shutdown or crash.
- Official Poky source is not tracked in this repository.
- This repository tracks only custom layers, recipes, scripts, and study notes.
