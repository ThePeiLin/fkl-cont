#include<fakeLisp/utils.h>
#include<fakeLisp/fklni.h>
#include<fakeLisp/vm.h>
#include<fakeLisp/opcode.h>
#include<fakeLisp/bytecode.h>
#include"cont.h"

void continuation_p(FKL_DL_PROC_ARGL)
{
	FKL_NI_BEGIN(exe);
	FklVMvalue* val=fklNiGetArg(&ap,stack);
	if(fklNiResBp(&ap,stack))
		FKL_RAISE_BUILTIN_ERROR_CSTR("continuation?",FKL_ERR_TOOFEWARG,exe);
	if(!val)
		FKL_RAISE_BUILTIN_ERROR_CSTR("continuation?",FKL_ERR_TOOFEWARG,exe);
	if(isCont(val))
		fklNiReturn(FKL_VM_TRUE,&ap,stack);
	else
		fklNiReturn(FKL_VM_NIL,&ap,stack);
	fklNiEnd(&ap,stack);
}

void call_cc(FKL_DL_PROC_ARGL)
{
	FKL_NI_BEGIN(exe);
	FklVMframe* frame=exe->frames;
	FklVMvalue* proc=fklNiGetArg(&ap,stack);
	if(fklNiResBp(&ap,stack))
		FKL_RAISE_BUILTIN_ERROR_CSTR("builtin.call/cc",FKL_ERR_TOOMANYARG,exe);
	if(!proc)
		FKL_RAISE_BUILTIN_ERROR_CSTR("builtin.call/cc",FKL_ERR_TOOFEWARG,exe);
	FKL_NI_CHECK_TYPE(proc,fklIsCallable,"builtin.call/cc",exe);
	Continuation* cc=createContinuation(ap,exe);
	if(!cc)
		FKL_RAISE_BUILTIN_ERROR_CSTR("builtin.call/cc",FKL_ERR_CROSS_C_CALL_CONTINUATION,exe);
	FklVMudata* ud=createContUd(cc,rel,pd);
	FklVMvalue* vcc=fklCreateVMvalueToStack(FKL_TYPE_USERDATA,ud,exe);
	fklNiSetBp(ap,stack);
	fklNiReturn(vcc,&ap,stack);
	fklTailCallobj(proc,frame,exe);
	fklNiEnd(&ap,stack);
}

static void cont_pd_finalizer(void* p)
{
	free(p);
}

static FklVMudMethodTable pdtable=
{
	.__append=NULL,
	.__atomic=NULL,
	.__cmp=NULL,
	.__copy=NULL,
	.__equal=NULL,
	.__prin1=NULL,
	.__princ=NULL,
	.__finalizer=cont_pd_finalizer,
};

void _fklInit(FklVMdll* rel,FklVM* exe)
{
	ContPublicData* pd=createContPd(fklAddSymbolCstr("continuation",exe->symbolTable)->id);
	FklVMvalue* ud=fklCreateVMvalueToStack(FKL_TYPE_USERDATA,fklCreateVMudata(0,&pdtable,pd,FKL_VM_NIL,FKL_VM_NIL),exe);
	fklSetRef(&rel->pd,ud,exe->gc);
}

struct SymFunc
{
	const char* sym;
	FklVMdllFunc f;
};

static const struct SymFunc
exports[]=
{
	{"continuation?", continuation_p, },
	{"call/cc",       call_cc,        },
};

static const size_t EXPORT_NUM=sizeof(exports)/sizeof(struct SymFunc);

void _fklExportSymbolInit(size_t* pnum,FklSid_t** psyms,FklSymbolTable* table)
{
	*pnum=EXPORT_NUM;
	FklSid_t* symbols=(FklSid_t*)malloc(sizeof(FklSid_t)*EXPORT_NUM);
	FKL_ASSERT(symbols);
	for(size_t i=0;i<EXPORT_NUM;i++)
		symbols[i]=fklAddSymbolCstr(exports[i].sym,table)->id;
	*psyms=symbols;
}

void _fklImportInit(FklVM* exe,FklVMvalue* dll,FklVMvalue* env)
{
	FklSymbolTable* table=exe->symbolTable;
	FklVMenv* e=env->u.env;
	for(size_t i=0;i<EXPORT_NUM;i++)
	{
		FklSid_t id=fklAddSymbolCstr(exports[i].sym,table)->id;
		FklVMdllFunc func=exports[i].f;
		FklVMdlproc* proc=fklCreateVMdlproc(func,dll,dll->u.dll->pd);
		proc->sid=id;
		FklVMvalue* dlproc=fklCreateVMvalueToStack(FKL_TYPE_DLPROC,proc,exe);
		fklFindOrAddVarWithValue(id,dlproc,e);
	}
}
