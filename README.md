# Yocto Labs

Hands-on Yocto Project experiments for custom layers, recipes, image configuration, and embedded Linux workflows.

## Goals

- Build a minimal Yocto image for QEMU
- Create custom layers and recipes
- Add user-space applications into an image
- Understand BitBake tasks, shared state cache, and image generation
- Document practical embedded Linux workflows

## Repository Layout

```text
notes/          Study notes and build logs
scripts/        Helper scripts
meta-dongwook/  Custom Yocto layer
```

## Local Build Layout

This repository does not include the official Poky source tree.

Recommended local workspace:

```text
~/yocto/poky/       Official Poky source tree
~/yocto/downloads/  Yocto download cache
~/yocto/sstate-cache/ Yocto shared state cache
~/yocto-labs/       Custom layer, notes, and scripts
```

## Initial Setup

Clone the official Poky source separately:

```bash
mkdir -p ~/yocto
cd ~/yocto
git clone -b scarthgap https://git.yoctoproject.org/poky
cd poky
```

Enter the build environment:

```bash
source oe-init-build-env build-qemux86-64
```

Build a minimal image:

```bash
bitbake core-image-minimal
```

Run the image with QEMU:

```bash
runqemu qemux86-64 core-image-minimal nographic
```

## Custom Layer

The custom layer is stored in this repository:

```text
~/yocto-labs/meta-dongwook/
```

Add it to the Yocto build:

```bash
cd ~/yocto/poky
source oe-init-build-env build-qemux86-64
bitbake-layers add-layer ~/yocto-labs/meta-dongwook
```

Check active layers:

```bash
bitbake-layers show-layers
```

## Notes

- Official Poky source is kept outside this repository.
- Build outputs, downloads, and shared state cache are not tracked by Git.
- This repository tracks only custom layers, recipes, scripts, and study notes.
