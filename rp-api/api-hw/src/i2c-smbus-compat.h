#ifndef I2C_SMBUS_COMPAT_H
#define I2C_SMBUS_COMPAT_H

/*
 * Modern linux/i2c-dev.h (kernel headers >= 5.x) no longer ships the
 * i2c_smbus_* inline helpers that older kernel-headers packages used to
 * provide alongside it. This restores the same small ioctl wrappers
 * (matching the historical i2c-tools implementation of the I2C_SMBUS
 * ioctl protocol) so this file keeps building unmodified against a
 * modern cross toolchain/sysroot.
 */

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                      int size, union i2c_smbus_data *data) {
    struct i2c_smbus_ioctl_data args;
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file, I2C_SMBUS, &args);
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value) {
    return ioctl(file, I2C_SMBUS_WRITE, value);
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value) {
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value) {
    union i2c_smbus_data data;
    data.word = value;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &data);
}

static inline __s32 i2c_smbus_read_byte(int file) {
    union i2c_smbus_data data;
    if (i2c_smbus_access(file, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
        return -1;
    return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command) {
    union i2c_smbus_data data;
    if (i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &data))
        return -1;
    return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_read_word_data(int file, __u8 command) {
    union i2c_smbus_data data;
    if (i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA, &data))
        return -1;
    return 0x0FFFF & data.word;
}

static inline __s32 i2c_smbus_write_block_data(int file, __u8 command, __u8 length,
                                                 const __u8 *values) {
    union i2c_smbus_data data;
    if (length > I2C_SMBUS_BLOCK_MAX)
        length = I2C_SMBUS_BLOCK_MAX;
    for (int i = 1; i <= length; i++)
        data.block[i] = values[i - 1];
    data.block[0] = length;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_DATA, &data);
}

static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length,
                                                     __u8 *values) {
    union i2c_smbus_data data;
    int i;
    if (length > I2C_SMBUS_BLOCK_MAX)
        length = I2C_SMBUS_BLOCK_MAX;
    data.block[0] = length;
    if (i2c_smbus_access(file, I2C_SMBUS_READ,
                          command,
                          length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA,
                          &data))
        return -1;
    for (i = 1; i <= data.block[0]; i++)
        values[i - 1] = data.block[i];
    return data.block[0];
}

#endif /* I2C_SMBUS_COMPAT_H */
