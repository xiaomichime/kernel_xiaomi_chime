// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/export.h>

/**
 * strncpy_from_user_nofault - Copy a NUL terminated string from unsafe user
 *                             address without triggering page faults.
 * @dst:         Destination buffer in kernel space.
 * @unsafe_addr: User-space pointer that may fault.
 * @count:       Maximum number of bytes to copy (including trailing NUL).
 *
 * Return:
 *  On success: length INCLUDING the trailing NUL.
 *  On fault:   -EFAULT.
 *  If count is smaller than string: returns count and ensures dst[count-1] = '\0'.
 */
long strncpy_from_user_nofault(char *dst, const void __user *unsafe_addr,
                               long count)
{
    long ret;

    if (unlikely(count <= 0))
        return 0;

    /* Disable pagefaults while copying from unsafe user address */
    pagefault_disable();
    ret = strncpy_from_user(dst, unsafe_addr, count);
    pagefault_enable();

    if (ret >= count) {
        /* Truncated: ensure trailing NUL */
        ret = count;
        dst[ret - 1] = '\0';
    } else if (ret > 0) {
        /* On success, include the NUL */
        ret++;
    }

    return ret;
}
EXPORT_SYMBOL(strncpy_from_user_nofault);
