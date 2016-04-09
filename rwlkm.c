#include <asm/atomic.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>

/* Общий ресурс */
static atomic_t *resource;

/* Количество потоков читателей и писателей */
#define rwlkm_readers 5
#define rwlkm_writers 1

/* Время блокировки потока в секундах (для снижения нагрузки на CPU) */
#define rwlkm_sleep 2

/* Указатели на потоки ядра */
static struct task_struct *rwlkm_reader_handlers[rwlkm_readers];
static struct task_struct *rwlkm_writer_handlers[rwlkm_writers];

/* Spin - блокировка чтения - записи */
DEFINE_RWLOCK(rwlkm_lock);

/* Функции потоков */
static int rwlkm_reader(void* usrdata) {

	while(1) {

		schedule_timeout_interruptible(rwlkm_sleep * HZ);

		if (!kthread_should_stop()) {

			read_lock(&rwlkm_lock);
			printk("PID %d: The reader has read the value: %d\n", current->pid, atomic_read(resource));
			read_unlock(&rwlkm_lock);

		} else {
			return 0;
		}

	}

}

static int rwlkm_writer(void* usrdata) {

	while(1) {

		schedule_timeout_interruptible(rwlkm_sleep * HZ);

		if (!kthread_should_stop()) {

			write_lock(&rwlkm_lock);
			atomic_inc(resource);
			printk("PID %d: The writer wrote the value: %d\n", current->pid, atomic_read(resource));
			write_unlock(&rwlkm_lock);

		} else {
			return 0;
		}

	}

}

static int __init rwlkm_init(void) {

	int i = 0;

	resource = (atomic_t *) vmalloc(sizeof(atomic_t));

	if (!resource) {
		return -ENOMEM;
	}

	atomic_set(resource, 0);

	for(i = 0; i < rwlkm_readers; i++) {
		rwlkm_reader_handlers[i] = kthread_run(&rwlkm_reader, NULL, "rwlkm_reader_%d", i);
	}

	for(i = 0; i < rwlkm_writers; i++) {
		rwlkm_writer_handlers[i] = kthread_run(&rwlkm_writer, NULL, "rwlkm_writer_%d", i);
	}

	return 0;

}

static void __exit rwlkm_exit(void) {

	int i = 0;

	vfree(resource);

	for(i = 0; i < rwlkm_readers; i++) {
		kthread_stop(rwlkm_reader_handlers[i]);
	}

	for(i = 0; i < rwlkm_writers; i++) {
		kthread_stop(rwlkm_writer_handlers[i]);
	}

}

MODULE_LICENSE("GPL");
module_init(rwlkm_init);
module_exit(rwlkm_exit);