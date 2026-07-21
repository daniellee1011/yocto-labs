// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/string.h>

#define EDU_VENDOR_ID       0x1234
#define EDU_DEVICE_ID       0x11e8
#define EDU_REG_ID          0x00
#define EDU_REG_LIVENESS    0x04
#define EDU_DMA_MASK_BITS   28
#define EDU_DMA_BUFFER_SIZE 4096

struct edu_device {
    struct pci_dev *pdev;
    void __iomem *bar0;

    void *dma_buffer;   // CPU virtual address; CPU accesses it
    dma_addr_t dma_handle;  // device/DMA address; hardware receives it
};

static int edu_probe(struct pci_dev *pdev,
        const struct pci_device_id *id) {
    struct edu_device *edu;
    u32 identification;
    u32 test_value = 0x12345678;
    u32 liveness;
    int ret;

    edu = devm_kzalloc(&pdev->dev, sizeof(*edu), GFP_KERNEL);
    if (!edu)
        return -ENOMEM;

    edu->pdev = pdev;

    ret = pci_enable_device_mem(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable PCI memory resources\n");
        return ret;
    }

    ret = dma_set_mask_and_coherent(&pdev->dev,
            DMA_BIT_MASK(EDU_DMA_MASK_BITS));
    if (ret) {
        dev_err(&pdev->dev, "Failed to configure 28-bit DMA mask\n");
        goto err_disable_device;
    }

    ret = pci_request_region(pdev, 0, "edu_dma");
    if (ret) {
        dev_err(&pdev->dev, "Failed to claim BAR0\n");
        goto err_disable_device;
    }

    edu->bar0 = pci_iomap(pdev, 0, 0);
    if (!edu->bar0) {
        dev_err(&pdev->dev, "Failed to map BAR0\n");
        ret = -ENOMEM;
        goto err_release_region;
    }

    identification = readl(edu->bar0 + EDU_REG_ID);

    writel(test_value, edu->bar0 + EDU_REG_LIVENESS);
    liveness = readl(edu->bar0 + EDU_REG_LIVENESS);

    if (liveness != ~test_value) {
        dev_err(&pdev->dev, "EDU liveness test failed\n");
        ret = -EIO;
        goto err_iounmap;
    }

    edu->dma_buffer = dma_alloc_coherent(&pdev->dev,
            EDU_DMA_BUFFER_SIZE, &edu->dma_handle, GFP_KERNEL);
    if (!edu->dma_buffer) {
        dev_err(&pdev->dev, "Failed to allocate coherent DMA buffer\n");
        ret = -ENOMEM;
        goto err_iounmap;
    }

    memset(edu->dma_buffer, 0, EDU_DMA_BUFFER_SIZE);

    pci_set_master(pdev);

    pci_set_drvdata(pdev, edu);

    dev_info(&pdev->dev, "BAR0 %pR\n", &pdev->resource[0]);
    dev_info(&pdev->dev, "identification=%#010x\n", identification);
    dev_info(&pdev->dev, "liveness=%#010x\n", liveness);
    dev_info(&pdev->dev, "DMA CPU address=%p\n", edu->dma_buffer);
    dev_info(&pdev->dev, "DMA bus address=%pad\n", &edu->dma_handle);

    return 0;

err_iounmap:
    pci_iounmap(pdev, edu->bar0);

err_release_region:
    pci_release_region(pdev, 0);

err_disable_device:
    pci_disable_device(pdev);

    return ret;
}

static void edu_remove(struct pci_dev *pdev) {
    struct edu_device *edu = pci_get_drvdata(pdev);

    pci_clear_master(pdev);

    dma_free_coherent(&pdev->dev,
            EDU_DMA_BUFFER_SIZE, edu->dma_buffer, edu->dma_handle);

    pci_iounmap(pdev, edu->bar0);
    pci_release_region(pdev, 0);
    pci_disable_device(pdev);

    dev_info(&pdev->dev, "EDU device removed\n");
}

static const struct pci_device_id edu_pci_ids[] = {
    { PCI_DEVICE(EDU_VENDOR_ID, EDU_DEVICE_ID) },
    { }
};

MODULE_DEVICE_TABLE(pci, edu_pci_ids);

static struct pci_driver edu_pci_driver = {
    .name = "edu_dma",
    .id_table = edu_pci_ids,
    .probe = edu_probe,
    .remove = edu_remove,
};

module_pci_driver(edu_pci_driver);

MODULE_AUTHOR("Daniel");
MODULE_DESCRIPTION("QEMU EDU PCI DMA learning driver");
MODULE_LICENSE("GPL");
