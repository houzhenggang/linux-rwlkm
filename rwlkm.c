#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>

/* Общий ресурс */
static int *resource;

/* Количество поток читателей и писателей */
#define rwlkm_readers 4
#define rwlkm_writers 4

/* Указатели на потоки ядра */
static struct task_struct *rwlkm_reader_handlers[rwlkm_readers];
static struct task_struct *rwlkm_writer_handlers[rwlkm_writers];

/* Функции потоков */
static int rwlkm_reader(void* usrdata) {

	while(!kthread_should_stop()) {}
	return 0;

}

static int rwlkm_writer(void* usrdata) {

	while(!kthread_should_stop()) {}
	return 0;

}

static int __init rwlkm_init(void) {

	/* Идентификатор потока ядра */
	const int rwlkm_handler_name_len = 32;
	char rwlkm_handler_name[rwlkm_handler_name_len];

	int i = 0;

	resource = (int *) vmalloc(sizeof(int));

	if (!resource) {
		return -ENOMEM;
	}

	for(i = 0; i < rwlkm_readers; i++) {
		snprintf(rwlkm_handler_name, rwlkm_handler_name_len, "rwlkm_reader_%d", i);
		rwlkm_reader_handlers[i] = kthread_run(&rwlkm_reader, NULL, rwlkm_handler_name);
	}

	for(i = 0; i < rwlkm_writers; i++) {
		snprintf(rwlkm_handler_name, rwlkm_handler_name_len, "rwlkm_writer_%d", i);
		rwlkm_writer_handlers[i] = kthread_run(&rwlkm_writer, NULL, rwlkm_handler_name);
	}

	return 0;

}

static void __exit rwlkm_exit(void) {

	int i = 0;

	vfree(resource);

	for(i = 0; i < rwlkm_readers; i++) {
		kthread_stop(rwlkm_reader_handlers[i]);
	}

	for(i = 0; i < rwlkm_readers; i++) {
		kthread_stop(rwlkm_writer_handlers[i]);
	}

}

MODULE_LICENSE("GPL");
module_init(rwlkm_init);
module_exit(rwlkm_exit);