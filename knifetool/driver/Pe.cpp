#include "Predef.h"
#include "Pe.h"

PIMAGE_NT_HEADERS GetNtHead(ULONG ImageBase)
{	
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_DOS_HEADER pDosHeader=(PIMAGE_DOS_HEADER)ImageBase;

	if (pDosHeader != NULL && pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		pNtHeader = (PIMAGE_NT_HEADERS)(ULONG)((ULONG)pDosHeader+pDosHeader->e_lfanew);
		if(pNtHeader != NULL && pNtHeader->Signature == IMAGE_NT_SIGNATURE)
		{
				
		}
		else
		{
			pNtHeader = NULL;
		}
	}

	return pNtHeader;
}


PIMAGE_SECTION_HEADER Rva2Section(PIMAGE_NT_HEADERS pNtHeader, ULONG rva)
{
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeader+sizeof(IMAGE_NT_HEADERS));
	
	for(int i = 0 ; i < pNtHeader->FileHeader.NumberOfSections; i++, pSec++)
	{
		if(rva >= pSec->VirtualAddress && rva <= pSec->VirtualAddress+pSec->SizeOfRawData)
		{
			return pSec;
		}
	}
	
	return  NULL;
	
}

DWORD Rva2Raw(PIMAGE_NT_HEADERS pNtHeader, ULONG rva)
{
	ULONG raw = 0;
	PIMAGE_SECTION_HEADER pSec = Rva2Section(pNtHeader, rva);	
	if(pSec != NULL)
	{
		raw = pSec->PointerToRawData + rva - pSec->VirtualAddress;
	}
	return raw;
}