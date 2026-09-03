# Game dump (Dumpspace)

Reference notes on the **Dumpspace** SDK dump for Splitgate — a public, machine-readable dump
of the game's Unreal Engine classes, structs, enums, functions, and global offsets. It's an
alternative source of the same information the hand-maintained SDK in [`Internal/ue/`](../Internal/ue/)
provides, and a handy cross-check when the game updates.

- **Browser (GUI):** https://dumpspace.spuckwaffel.com/Games/?hash=d2a5bd8c
- **Raw files:** https://github.com/Spuckwaffel/dumpspace/tree/main/Games/Unreal-Engine-4/Splitgate
- **Produced by:** [UEDumper](https://github.com/Spuckwaffel/UEDumper); the dump `version` seen
  here is `10201`.
- **Reference API wrapper:** [Dumpspace-API `DSAPI`](https://github.com/Spuckwaffel/Dumpspace-API)
  (`DSAPI.h` / `DSAPI.cpp`) — a small C++ class that downloads and queries these files.

> Offsets are absolute values for **one specific build** of the game. They change every game
> update, so treat anything here as a snapshot keyed by the dump's `updated_at`/`version`, not a
> constant.

## Files

Each dump type ships as raw JSON plus a gzipped copy (`*.json.gz`); there is also a preview
`image.jpg`.

| File | Size | Contents |
| ---- | ---- | -------- |
| `OffsetsInfo.json`   | ~313 B | global offsets (GNames / GObjects / GWorld) |
| `EnumsInfo.json`     | ~0.5 MB | enum name → value maps |
| `StructsInfo.json`   | ~1.2 MB | non-`UObject` struct layouts |
| `ClassesInfo.json`   | ~2.8 MB | `UObject`-derived class layouts |
| `FunctionsInfo.json` | ~3.0 MB | per-class function addresses |

Raw base URL:
`https://raw.githubusercontent.com/Spuckwaffel/dumpspace/main/Games/Unreal-Engine-4/Splitgate/<file>`

## Global offsets (this snapshot)

`OffsetsInfo.json` is tiny and the most immediately useful — the three engine globals, as
RVAs into the game module:

| Name | Value (dec) | Value (hex) |
| ---- | ----------- | ----------- |
| `OFFSET_GNAMES`   | 91336512 | 0x571E100 |
| `OFFSET_GOBJECTS` | 91583728 | 0x575A6B0 |
| `OFFSET_GWORLD`   | 92916576 | 0x589FCE0 |

```json
{
  "credit": { "dumper_link": "https://github.com/Spuckwaffel/UEDumper", "dumper_used": "UEDumper" },
  "data": [ ["OFFSET_GNAMES", 91336512], ["OFFSET_GOBJECTS", 91583728], ["OFFSET_GWORLD", 92916576] ],
  "updated_at": "1760304743193",
  "version": 10201
}
```

## JSON schema

Every file shares one envelope:

```json
{ "version": 10201, "data": [ ... ] }
```

The parser reads `version` first (`fileVersion = json.value("version", 0)`) because it controls
the member-tuple length (see bitfields below).

### OffsetsInfo

`data` is an array of `[name, value]` pairs. DSAPI stores them as `offsetMap[name] = value` and
exposes `uint64_t getOffset(name)`.

### ClassesInfo / StructsInfo

Same shape (Classes = `UObject`-derived, Structs = plain). `data` is an array of one-key objects
`{ "<TypeName>": [ ...entries ] }`. Each entry is a two-element `[key, value]`:

- `["__InheritInfo", [ ...base type names... ]]` — inheritance chain.
- `["__MDKClassSize", <int>]` — `sizeof` the type.
- `["<MemberName>", [ <typeInfo>, <offset>, <size> ]]` — a data member. When the member is a
  **bitfield**, the tuple gains a trailing bit offset (length 4 at `version` 10201, length 5 at
  10202) — the parser detects this by the inner array's length.

`<typeInfo>` is itself an array `[typeName, category, modifier, subTypes]`, e.g. a plain int is:

```json
"ComparisonIndex": [ ["int", "D", "", []], 0, 4 ]
```

i.e. member `ComparisonIndex` at **offset 0**, **size 4**, type `int`. `category` is a short
code for the type kind (primitive / pointer / class / enum / struct — see UEDumper for the full
set), and `subTypes` holds template/inner types (e.g. the element type of a `TArray`). Example
top of `StructsInfo.json`:

```json
{ "data": [ { "FName": [
  { "__InheritInfo": [] },
  { "__MDKClassSize": 8 },
  { "ComparisonIndex": [ ["int", "D", "", []], 0, 4 ] }
] } ] }
```

DSAPI only consumes `offset`/`size`/`bitOffset` — it ignores `typeInfo` — keying members as
`classMemberMap[className + memberName]` and sizes as `classSizeMap[className]`.

### FunctionsInfo

`data` is an array of one-key objects `{ "<ClassName>": [ entries ] }`, each entry
`["<FunctionName>", [ idx0, idx1, <addressUint64> ]]`. DSAPI takes index `[2]` as the function
address and keys it as `functionOffsetMap[className + functionName]`.

### EnumsInfo

`data` is an array of one-key objects `{ "<EnumName>": [ { "<ValueName>": <int>, ... } ] }`.
DSAPI stores each as `enumNameMap[enumName + to_string(value)] = valueName` for reverse lookup.

## The reference wrapper (`DSAPI`)

`DSAPI` (from the Dumpspace-API repo) is the canonical example of consuming these files:

```cpp
DSAPI(const std::string& gameHash);                 // e.g. "d2a5bd8c" for Splitgate
void downloadContent(contentTypes types = all);     // fetch + parse the JSONs

OffsetInfo getOffset(const std::string& className,  // member offset/size/bitfield
                     const std::string& memberName);
int        getSizeofClass(const std::string& className);
uint64_t   getFunctionOffset(const std::string& fnClass, const std::string& fnName);
std::string getEnumName(const std::string& enumClass, uint64_t value);
uint64_t   getOffset(const std::string& name);      // GNames / GObjects / GWorld / ...
```

```cpp
struct OffsetInfo { uint64_t offset, size; bool isBit, valid; int bitOffset; };
enum class contentTypes { classes, structs, enums, functions, offsets, all };
```

It resolves the game hash against a top-level `GameList.json`, downloads the five files, parses
each into an `nlohmann::json`, and flattens them into the composite-key maps above. Lookups are
then O(1) map hits on `className + memberName` (etc.).

## Relation to this project

`Internal/ue/` already carries a hand-maintained UE SDK, and offsets are resolved at runtime
(the injection path finds `World`/`GObjects` via the engine rather than hard-coded RVAs, so the
mod survives some updates). The dump is useful here as:

- a **reference** to cross-check member offsets / sizes / function addresses when adding to the
  SDK or chasing a post-update crash;
- the source of `GNames`/`GObjects`/`GWorld` RVAs if a static-offset path is ever needed;
- input for a potential **offset generator** — because the schema is small and regular, a build
  step (or a `DSAPI`-style helper) could pull the JSON and emit offsets rather than editing the
  SDK by hand. (Not built; noted as an option.)
