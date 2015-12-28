# vmfs

From minimal Debian 8 install:

1. apt-get install make gcc fuse libfuse-dev fuse-utils pkg-config git libssl-dev
2. git clone https://github.com/douglastechnology/vmfs.git /root/vmfs
3. gcc -std=gnu99 -Wall /root/vmfs/rw.c /root/vmfs/murmur3.c /root/vmfs/lz4.c /root/vmfs/aes.c /root/vmfs/vmfs.c \`pkg-config fuse --cflags --libs\` -lcrypto -o /usr/local/bin/vmfs
4. gcc -std=gnu99 -Wall /root/vmfs/rw.c /root/vmfs/murmur3.c /root/vmfs/lz4.c /root/vmfs/aes.c /root/vmfs/basis.c -lcrypto -o /usr/local/bin/basis
5. gcc -std=gnu99 -Wall /root/vmfs/vm.c -o /usr/local/bin/vm
6. mkdir /root/hash
7. mkdir /root/vm
8. basis
9. vm myvm 1
10. vmfs /mnt
11. time dd if=/mnt/myvm bs=32K count=32K of=/dev/null
12. umount /mnt
