# iostream的用途与局限

## c stdio 格式化输出的缺点

### 对编程初学者不友好

- 输入和输出用的格式化字符串不一样。比如 `short` 输入时用 `%hd`，输出却用 `%d`;
- 输入参数不统一。比如对于`int, long, double`等类型，在传入 `scanf()` 的时候要取地址(&), 而对于字符数组，则无需取地址，直接用数组名；
- 缓冲区存在溢出的风险。

### 安全性

对于输出安全性，自从C99 增加了 `snprintf()` 等能够指定输出缓冲区大小的函数，输出方面的安全性问题已经得到了解决。

而关于输入安全性，似乎没有太大进展，仍需程序员自己动手。比如从文件读取一行字符串，行的长度不固定。我们可以选择的方式有以下几种：

- `gets()`：不能指定缓冲区长度，存在缓冲区溢出的风险;
- `fgets()`：可以指定缓冲区长度，所以是安全的。但程序员必须预设一个长度的最大值;
- `getline()`：可以用来读取不定长的行，无需指定缓冲区长度。该函数返回一个缓冲区地址，需要用户手动调用 `free` 去释放;

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  FILE *stream;
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  stream = fopen(argv[1], "r");
  if (stream == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while ((nread = getline(&line, &len, stream)) != -1) {
    printf("Retrieved line of length %zd:\n", nread);
    fwrite(line, nread, 1, stdout);
  }

  free(line);
  fclose(stream);
  exit(EXIT_SUCCESS);
}
```

### 类型安全

- 对于使用 `typedef` 重命名的类型，不能一眼看出其具体类型；
- 有时不能够同时兼容 32-bit 和 64-bit 平台；
- 格式化字符串和参数类型不匹配会造成难以发现的 bug，即使加了`-Wall`, **程序员可能选择性地忽略掉**。

### 不可扩展

c stdio 不支持自定义的类型。

## 参考

[1] [getline(3) — Linux manual page](https://man7.org/linux/man-pages/man3/getline.3.html)
