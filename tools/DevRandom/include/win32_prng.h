//-----------------------------------------------------------------------------
// Win32 PRNG
//-----------------------------------------------------------------------------

typedef BOOLEAN(*PRtlGenRandom)(PVOID RandomBuffer, ULONG RandomBufferLength);

typedef struct
{
	HMODULE hModAdvapi32;
	PRtlGenRandom rtlGenRandom;
}
rand_t;

static void rand_init(rand_t *const ctx)
{
	ctx->hModAdvapi32 = LoadLibraryA("Advapi32.dll");
	ctx->rtlGenRandom = ctx->hModAdvapi32 ? (PRtlGenRandom)GetProcAddress(ctx->hModAdvapi32, "SystemFunction036") : NULL;
	if (!ctx->rtlGenRandom)
	{
		ExitProcess(-1); /*system error detected*/
	}
}

static inline rand_bytes(const rand_t *const ctx, BYTE *const buffer, ULONG size)
{
	if (!ctx->rtlGenRandom(buffer, sizeof(BYTE) * size))
	{
		ExitProcess(-1); /*system error detected*/
	}
}

static inline rand_close(rand_t *const ctx)
{
	ctx->rtlGenRandom = NULL;
	if (ctx->hModAdvapi32)
	{
		FreeLibrary(ctx->hModAdvapi32);
		ctx->hModAdvapi32 = NULL;
	}
}
