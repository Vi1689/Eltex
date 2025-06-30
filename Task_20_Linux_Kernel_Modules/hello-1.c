#include <linux/module.h> /* Needed by all modules */

#include <linux/kernel.h> /* Needed for pr_info() */

int init_module(void)

{

  pr_info("My test module loaded!!!!!\n");

  /* A nonzero return means init_module failed; module can't be loaded. */

  return 0;
}

void cleanup_module(void)

{

  pr_info("My test module unloaded!!!!!\n");
}

MODULE_LICENSE("GPL");