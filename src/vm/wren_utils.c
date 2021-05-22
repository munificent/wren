#include <string.h>

#include "wren_utils.h"
#include "wren_vm.h"

DEFINE_BUFFER(Byte, uint8_t);
DEFINE_BUFFER(Int, int);
DEFINE_BUFFER(String, ObjString*);

void wrenSymbolTableInit(SymbolTable* symbols)
{
    symbols->hSize = 0;
    symbols->hCapacity = WREN_ST_DEFAULT_CAPACITY;

    // Nothing is set at first, so call calloc() to initialize the hashSet
    symbols->hashSet = calloc(WREN_ST_DEFAULT_CAPACITY, sizeof(HashIdx*));

    wrenStringBufferInit(&symbols->objs);
}

inline static void freeHashIdx(WrenVM *vm, HashIdx *symbol)
{
    if (!symbol) return;

    HashIdx *next = symbol->next;

    DEALLOCATE(vm, symbol);

    if (next) freeHashIdx(vm, next);
}

static void wrenSymbolTableClearBitset(WrenVM *vm, SymbolTable *symbols)
{
    for (size_t i = 0; i < symbols->hCapacity; i++)
    {
        freeHashIdx(vm, symbols->hashSet[i]);
        symbols->hashSet[i] = NULL;
    }

    symbols->hSize = 0;
}

void wrenSymbolTableClear(WrenVM* vm, SymbolTable* symbols)
{
    // Reset the number of hashes to 0
    symbols->hSize = 0;

    // Clear the hashSet, then the buffer
    wrenSymbolTableClearBitset(vm, symbols);
    wrenStringBufferClear(vm, &symbols->objs);

    DEALLOCATE(vm, symbols->hashSet);
}

static unsigned long wrenHashDjb2(const char *key, size_t length)
{
    unsigned long hash = 5381; // Magic starting point

    for (size_t i = 0; i < length; i++)
        hash = ((hash << 5) + hash) + key[i];

    return hash;
}

static HashIdx *wrenSymbolInit(WrenVM *vm, size_t buffIdx, HashIdx *next)
{
    HashIdx *newSymbol = ALLOCATE(vm, HashIdx);

    newSymbol->idx = buffIdx;
    newSymbol->next = next;

    return newSymbol;
}

//Forward declare Grow() since InsertHash() might call it
static void wrenSymbolTableGrow(WrenVM *vm, SymbolTable *symbols);

// Insert a symbol in the hashSet with a specific index
static size_t wrenSymbolTableInsertHashIdx(WrenVM *vm, SymbolTable *symbols,
                                           ObjString *symbol, size_t idx)
{
    size_t hashIdx = wrenHashDjb2(symbol->value, symbol->length) % symbols->hCapacity;

    symbols->hSize++;

    // "Initialize" the element in the set
    symbols->hashSet[hashIdx] = wrenSymbolInit(vm, idx, symbols->hashSet[hashIdx]);

    return idx;
}

static size_t wrenSymbolTableInsertHash(WrenVM *vm, SymbolTable *symbols,
                                      ObjString *symbol)
{
    // Compute the true index and the hash index
    size_t buffIdx = symbols->objs.count;

    return wrenSymbolTableInsertHashIdx(vm, symbols, symbol, buffIdx);
}

static void wrenSymbolTableGrow(WrenVM *vm, SymbolTable *symbols)
{
    // We need to clear the hashSet and re-hash all symbols
    wrenSymbolTableClearBitset(vm, symbols);

    size_t oldCap = symbols->hCapacity;
    symbols->hCapacity *= 2;
    symbols->hashSet = wrenReallocate(vm,
                                      symbols->hashSet,
                                      oldCap * sizeof(HashIdx *),
                                      symbols->hCapacity * sizeof(HashIdx*));

    // Initialize the new elements in the hashSet
    memset(symbols->hashSet + oldCap, 0x0, (symbols->hCapacity - oldCap) * sizeof(HashIdx*));

    // Re-hash all the symbols
    for (size_t i = 0; i < symbols->objs.count; i++)
        wrenSymbolTableInsertHashIdx(vm, symbols, symbols->objs.data[i], i);
}

int wrenSymbolTableAdd(WrenVM* vm, SymbolTable* symbols,
                       const char* name, size_t length)
{
    ObjString* symbol = AS_STRING(wrenNewStringLength(vm, name, length));

    wrenPushRoot(vm, &symbol->obj);

    size_t buffIdx = wrenSymbolTableInsertHash(vm, symbols, symbol);

    // Add the name to the buffer
    wrenStringBufferWrite(vm, &symbols->objs, symbol);

    if (symbols->hSize == symbols->hCapacity)
        wrenSymbolTableGrow(vm, symbols);

    wrenPopRoot(vm);

    return buffIdx;
}

int wrenSymbolTableEnsure(WrenVM* vm, SymbolTable* symbols,
                          const char* name, size_t length)
{
    // See if the symbol is already defined.
    int existing = wrenSymbolTableFind(symbols, name, length);
    if (existing != -1) return existing;

    // New symbol, so add it.
    return wrenSymbolTableAdd(vm, symbols, name, length);
}

static int wrenSymbolTableFindCollision(const SymbolTable *symbols, const HashIdx *head,
                                 const char *name, size_t length)
{
    if (!head)
        return -1;

    if (wrenStringEqualsCString(symbols->objs.data[head->idx], name, length))
        return head->idx;

    return wrenSymbolTableFindCollision(symbols, head->next, name, length);
}

int wrenSymbolTableFind(const SymbolTable* symbols,
                        const char* name, size_t length)
{
    size_t hashIdx = wrenHashDjb2(name, length) % symbols->hCapacity;

    if (!symbols->hashSet[hashIdx])
        return -1;

    return wrenSymbolTableFindCollision(symbols, symbols->hashSet[hashIdx], name, length);
}

void wrenBlackenSymbolTable(WrenVM* vm, SymbolTable* symbolTable)
{
  for (int i = 0; i < symbolTable->objs.count; i++)
  {
    wrenGrayObj(vm, &symbolTable->objs.data[i]->obj);
  }
  
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += symbolTable->objs.capacity * sizeof(*symbolTable->objs.data);
}

int wrenUtf8EncodeNumBytes(int value)
{
  ASSERT(value >= 0, "Cannot encode a negative value.");
  
  if (value <= 0x7f) return 1;
  if (value <= 0x7ff) return 2;
  if (value <= 0xffff) return 3;
  if (value <= 0x10ffff) return 4;
  return 0;
}

int wrenUtf8Encode(int value, uint8_t* bytes)
{
  if (value <= 0x7f)
  {
    // Single byte (i.e. fits in ASCII).
    *bytes = value & 0x7f;
    return 1;
  }
  else if (value <= 0x7ff)
  {
    // Two byte sequence: 110xxxxx 10xxxxxx.
    *bytes = 0xc0 | ((value & 0x7c0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 2;
  }
  else if (value <= 0xffff)
  {
    // Three byte sequence: 1110xxxx 10xxxxxx 10xxxxxx.
    *bytes = 0xe0 | ((value & 0xf000) >> 12);
    bytes++;
    *bytes = 0x80 | ((value & 0xfc0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 3;
  }
  else if (value <= 0x10ffff)
  {
    // Four byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
    *bytes = 0xf0 | ((value & 0x1c0000) >> 18);
    bytes++;
    *bytes = 0x80 | ((value & 0x3f000) >> 12);
    bytes++;
    *bytes = 0x80 | ((value & 0xfc0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 4;
  }

  // Invalid Unicode value. See: http://tools.ietf.org/html/rfc3629
  UNREACHABLE();
  return 0;
}

int wrenUtf8Decode(const uint8_t* bytes, uint32_t length)
{
  // Single byte (i.e. fits in ASCII).
  if (*bytes <= 0x7f) return *bytes;

  int value;
  uint32_t remainingBytes;
  if ((*bytes & 0xe0) == 0xc0)
  {
    // Two byte sequence: 110xxxxx 10xxxxxx.
    value = *bytes & 0x1f;
    remainingBytes = 1;
  }
  else if ((*bytes & 0xf0) == 0xe0)
  {
    // Three byte sequence: 1110xxxx	 10xxxxxx 10xxxxxx.
    value = *bytes & 0x0f;
    remainingBytes = 2;
  }
  else if ((*bytes & 0xf8) == 0xf0)
  {
    // Four byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
    value = *bytes & 0x07;
    remainingBytes = 3;
  }
  else
  {
    // Invalid UTF-8 sequence.
    return -1;
  }

  // Don't read past the end of the buffer on truncated UTF-8.
  if (remainingBytes > length - 1) return -1;

  while (remainingBytes > 0)
  {
    bytes++;
    remainingBytes--;

    // Remaining bytes must be of form 10xxxxxx.
    if ((*bytes & 0xc0) != 0x80) return -1;

    value = value << 6 | (*bytes & 0x3f);
  }

  return value;
}

int wrenUtf8DecodeNumBytes(uint8_t byte)
{
  // If the byte starts with 10xxxxx, it's the middle of a UTF-8 sequence, so
  // don't count it at all.
  if ((byte & 0xc0) == 0x80) return 0;
  
  // The first byte's high bits tell us how many bytes are in the UTF-8
  // sequence.
  if ((byte & 0xf8) == 0xf0) return 4;
  if ((byte & 0xf0) == 0xe0) return 3;
  if ((byte & 0xe0) == 0xc0) return 2;
  return 1;
}

// From: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
int wrenPowerOf2Ceil(int n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  
  return n;
}

uint32_t wrenValidateIndex(uint32_t count, int64_t value)
{
  // Negative indices count from the end.
  if (value < 0) value = count + value;

  // Check bounds.
  if (value >= 0 && value < count) return (uint32_t)value;

  return UINT32_MAX;
}
