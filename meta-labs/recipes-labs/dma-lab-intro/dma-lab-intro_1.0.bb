SUMMARY = "Introductory userspace program for the DMA lab"
DESCRIPTION = "Prints a CPU virtual address and the system page size."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://dma_lab_intro.c"

S = "${WORKDIR}"

do_compile() {
    ${CC} ${CFLAGS} ${S}/dma_lab_intro.c ${LDFLAGS} -o dma-lab-intro
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 dma-lab-intro ${D}${bindir}/dma-lab-intro
}
