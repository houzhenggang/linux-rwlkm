#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/vmalloc.h>

static int __init rwlkm_init(void) {
	return 0;
}

static void __exit rwlkm_exit(void) {
	return;
}

MODULE_LICENSE("GPL");
module_init(rwlkm_init);
module_exit(rwlkm_exit);