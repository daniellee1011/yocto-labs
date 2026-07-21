SUMMARY = "Minimal image for embedded Linux labs"
DESCRIPTION = "A reusable QEMU image containing programs and tools for hands-on labs."

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL:append = " dma-lab-intro pciutils edu-dma"
