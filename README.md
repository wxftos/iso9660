A header only library for parsing ISO9660 filesystem.

It's interface is not good enough.

Some usefull references in regard of ISO9660.

- https://wiki.osdev.org/ISO_9660
- https://wiki.osdev.org/El-Torito
- https://pdos.csail.mit.edu/6.828/2014/readings/boot-cdrom.pdf
- https://www.cdroller.com/htm/readdata.html
- https://en.wikipedia.org/wiki/ISO_9660
- https://habr.com/ru/companies/ruvds/articles/706070/
- https://en.wikipedia.org/wiki/BIOS_parameter_block
- https://stackoverflow.com/questions/9731323/loading-a-file-on-an-iso-9660-file-system
- https://stackoverflow.com/questions/52525272/how-can-i-read-a-directory-on-iso9660-from-the-path-table-when-the-table-does-no
- https://386bsd.org/releases/inside-the-iso9660-filesystem-format-untangling-cdrom-standards-article

```make
# to run an example
make
```

##### todo

1. Make a more friendly interface (not sure yet what it would look like).
2. Ability to dump a read/build FS back to a file.
3. An easier way to build a FS.