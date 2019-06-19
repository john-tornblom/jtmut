# Johns tiny schemata mutation testing tool
jtmut is a tiny schemata mutation testing tool for xml-based srcml format.
For now though, only C programs are considered.

##  Building the Run-time Library
```console
john@localhost:jtmut$ make
```

## Generating a metaprogram
jtmut operator on a file format called srcml. For instructions on how to translate
for source code into this file format, see [srcml.org](https://www.srcml.org).

Once you have obtained a srcml file, you can generate a meta-program as follows:
```console
john@localhost:jtmut$ ./jtmut.py source.xml
```

##  Running the Triangle Example
```console
john@localhost:jtmut$ make -f examples/triangle/Makefile
```

## Reporting Bugs
If you encounter problems with jtmut, please [file a github issue][issues]. If
you plan on sending pull requests which affect more than a few lines of code,
please file an issue before you start to work on you changes. This will allow us
to discuss the solution properly before you commit time and effort.

## License
jtmut is licensed under the LGPLv3+.

[issues]: https://github.com/john-tornblom/jtmut/issues/new
