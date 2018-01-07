# open-windows: open command for windows.

open-windows is simple utility to open the file with associated program.

## Motivation

There is `start` command in cmd.exe:

```
> start foo.txt
```

can open foo.txt as if it was double-clicked in the explorer. But this start
command has some trouble with spaces. for example,

```
> start "foo bar.txt"
```

doesn't start `foo bar.txt`, instead launch new console titled "foo bar.txt".
There is well-known workaround:

```
> start "dummy" "foo bar.txt"
```

This works (double quotation mark around dummy is important; if there isn't,
windows would complain about missing file named dummy). 

Then, I wanted to use this in my rust program:

```rust
    Command::new("cmd")
        .arg("/c")
        .arg("start")
        .arg("dummy")
        .arg("foo bar.txt")
        .output()
        .unwrap();
```

But this didn't achieve what I wanted since `dummy` was not double-quoted. However,

```rust
    Command::new("cmd")
        .arg("/c")
        .arg("start")
        .arg("\"dummy\"")
        .arg("foo bar.txt")
        .output()
        .unwrap();
```

produced error that there was no file named `\\dummy\\`. I gave up and decided
to make a new program to simply open the specified file collectly.

## Usage

Simple. You can use it as follows:

```
> open "foo bar.txt"
```

and it can open `foo bar.txt`. good.

available options are `-e`, `-h`.

`-e` changes the `lpVerb` parameter of ShellExecute() from default `open` to
`explore`. But if we specify a folder name instead of a file name we can open
the folder in explorer.exe **with no option**, so this option seems to be
useless. 

`-h` shows usage. It will be displayed using three successive message box.
