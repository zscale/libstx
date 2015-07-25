# libxzero-fnord API adaptions

## Allgemeines

- verzeichnis prefix startet mit `xzero-` anstatt `fnord-`.
- cmake: es werden static libraries (zB .a files) erzeugt anstatt OBJECT files alleine.
- include-header werden `CamelCase.h` wie die Klasse geschrieben, nicht `<lowercase.h>`.
- unit tests nutzen gtest (google test) library

## Was so geblieben ist

- base:
  - FNV
  - Random
  - Uri
  - Application
  - thread::SignalHandler
  - thread::Wakeup
  - thread::Queue

## Duration

Neuer Name: `TimeSpan`.

```Duraction.microseconds``` to ```TimeSpan.totalMicroseconds```

## Reflection

basically 1:1 uebernommen, jedoch ist der toplevel include header auch
im toplevel directory, also `<xzero-base/reflect.h>`.

## Logging

`#include <xzero-base/logging.h>` wie in fnord; jedoch exceptions
werden via `logError(string component, std::exception& e)` geloggt.

## Exceptions

`#include <xzero-base/RuntimeError.h>` um die standard exception base class,
`RuntimeError` und deren Helfer zu includen.

`RuntimeError.appendMessage()` wurde nicht mit uebernommen.

`RAISE(E, ...)` ist *NICHT* wie in libfnord, da `E` nun ein echter typ sein
muss. Die alte behaviour wird via `RAISE_FAKE(E, ...)` erzielt, hier
ist `E` ein string.

`RAISE_ERRNO(errno)` bleibt as-is.

`BUG_ON(cond)` ist neu, und raised wenn `cond` zu `false` evaluated.

## Exception Handler

EHs werden nicht mehr via fixed Interface Klasse implementiert,
sondern anything dass auf `std::function<void(...)>` matched.

EH Naming Changes:

- `CatchAndLogException` => `void logAndPass(const std::exception& e)`
- `CatchAndAbortException` => `void logAndAbort(const std::exception& e)`

Global exception handler wird installiert via:

```
int main() {
  Application::installGlobalExceptionHandler();
  // ...
}
```

## Buffer / String Management

...

## Assets

Ist _formschoener_ implementiert, via `MemoryFile` und `MemoryFileRepository`.
Siehe example code.

## I/O

`File` ist in libfnord eigentlich ein stream (/channel). in libxzero
werden dafuer `std::istream` und `std::ostream` verwendet.

Wird das *sufficient* sein?

`File` in libxzero ist die Metainformation zu einer file, und implementiert
durch `LocalFile` fuer lokale files on disk, und `MemoryFile` fuer
kuenstlich erstellte files (aka libfnord's Assets).

Dadurch koennen dieselben Code Paths fuer egal welche File I/O Kathegorien
genutzt werden.

`FileRef` aus libfnord gibt es nicht mehr, stattdessen kann dessen einziger
Nutzer aus `LocalFileRepository.createTempFile(...)` den alten inhalt durch
2 codezeilen mehr inferen. (siehe `LocalFileRepository.baseDirectory()`).

`MmappedFile` heisst jetzt `MemoryMap` und kann via `File.createMemoryMap()`
von einer file erstellt werden.

Ein file descriptor kann zB wiefolgt geoeffnet werden:

```
  FileDescriptor fd = myfile.createPosixChannel(File::Read | File::NonBlocking);
  std::cout << FileUtil::read(fd) << std::endl;
```

## FreeOnDestroy

Kann auch via stdlib gehackt werden:
```
{
  std::shared_ptr<void> p1(malloc(42), [](void* p) { free(p); });
  printf("p1: @%p\n", p1.get());

  std::shared_ptr<void> p2(malloc(42), std::bind(&free, std::placeholders::_1));
  printf("p2: @%p\n", p2.get());
}
```

### Option Type

`None<T>()` wird zu `None()` - das reicht.

### Wakeup

ist jetzt in `xzero::thread` anstatt nur im main namespace (aka: `fnord`)

