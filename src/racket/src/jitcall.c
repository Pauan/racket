/*
  Racket
  Copyright (c) 2006-2011 PLT Scheme Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301 USA.
*/


#include "schpriv.h"
#include "schmach.h"
#include "future.h"

#ifdef MZ_USE_JIT

#include "jit.h"

int scheme_direct_call_count, scheme_indirect_call_count;

THREAD_LOCAL_DECL(static Scheme_Object **fixup_runstack_base);
THREAD_LOCAL_DECL(static int fixup_already_in_place);

static Scheme_Object *clear_runstack(Scheme_Object **rs, intptr_t amt, Scheme_Object *sv)
{
  int i;
  for (i = 0; i < amt; i++) {
    rs[i] = NULL;
  }
  return sv;
}

#define JITCALL_TS_PROCS
#define JIT_APPLY_TS_PROCS
#include "jit_ts.c"

/*========================================================================*/
/*                         application codegen                            */
/*========================================================================*/

static jit_insn *generate_proc_struct_retry(mz_jit_state *jitter, int num_rands, GC_CAN_IGNORE jit_insn *refagain)
{
  GC_CAN_IGNORE jit_insn *ref2, *refz1, *refz2, *refz3, *refz4, *refz5;

  ref2 = jit_bnei_i(jit_forward(), JIT_R1, scheme_proc_struct_type);
  jit_ldxi_p(JIT_R1, JIT_V1, &((Scheme_Structure *)0x0)->stype);
  jit_ldi_p(JIT_R2, &scheme_reduced_procedure_struct);
  refz3 = jit_beqr_p(jit_forward(), JIT_R1, JIT_R2);
  jit_ldxi_p(JIT_R1, JIT_R1, &((Scheme_Struct_Type *)0x0)->proc_attr);
  refz1 = jit_bmci_i(jit_forward(), JIT_R1, 0x1);
  CHECK_LIMIT();

  /* Proc is a field in the record */
  jit_rshi_ul(JIT_R1, JIT_R1, 1);
  jit_lshi_ul(JIT_R1, JIT_R1, JIT_LOG_WORD_SIZE);
  jit_addi_p(JIT_R1, JIT_R1, &((Scheme_Structure *)0x0)->slots);
  jit_ldxr_p(JIT_R1, JIT_V1, JIT_R1);

  /* JIT_R1 now has the wrapped procedure */
  refz4 = jit_bmsi_i(jit_forward(), JIT_R1, 0x1);
  jit_ldr_s(JIT_R2, JIT_R1);
  refz2 = jit_bnei_i(jit_forward(), JIT_R2, scheme_native_closure_type);
  CHECK_LIMIT();

  /* It's a native closure, but we can't just jump to it, in case
     the arity is wrong. */
  mz_prepare(2);
  jit_movi_i(JIT_R0, num_rands);
  jit_pusharg_i(JIT_R0); /* argc */
  jit_pusharg_p(JIT_R1); /* closure */
  (void)mz_finish(scheme_native_arity_check);
  CHECK_LIMIT();
  jit_retval(JIT_R0);
  refz5 = jit_beqi_i(jit_forward(), JIT_R0, 0);
  CHECK_LIMIT();

  /* Extract proc again, then loop */
  jit_ldxi_p(JIT_R1, JIT_V1, &((Scheme_Structure *)0x0)->stype);
  jit_ldxi_p(JIT_R1, JIT_R1, &((Scheme_Struct_Type *)0x0)->proc_attr);
  jit_rshi_ul(JIT_R1, JIT_R1, 1);
  jit_lshi_ul(JIT_R1, JIT_R1, JIT_LOG_WORD_SIZE);
  jit_addi_p(JIT_R1, JIT_R1, &((Scheme_Structure *)0x0)->slots);
  jit_ldxr_p(JIT_V1, JIT_V1, JIT_R1);
  (void)jit_jmpi(refagain);
  CHECK_LIMIT();

  mz_patch_branch(refz1);
  mz_patch_branch(refz2);
  mz_patch_branch(refz3);
  mz_patch_branch(refz4);
  mz_patch_branch(refz5);

  return ref2;
}

#ifdef INSTRUMENT_PRIMITIVES
extern int g_print_prims;
#endif

/* Support for intercepting direct calls to primitives: */
#ifdef MZ_USE_FUTURES

Scheme_Object *scheme_noncm_prim_indirect(Scheme_Prim proc, int argc) 
  XFORM_SKIP_PROC
{
  if (scheme_use_rtcall)
    return scheme_rtcall_iS_s("[prim_indirect]",
                              FSRC_PRIM,
                              proc, 
                              argc, 
                              MZ_RUNSTACK);
  else 
    return proc(argc, MZ_RUNSTACK);
}

Scheme_Object *scheme_prim_indirect(Scheme_Primitive_Closure_Proc proc, int argc, Scheme_Object *self) 
  XFORM_SKIP_PROC
{
  if (scheme_use_rtcall)
    return scheme_rtcall_iSs_s("[prim_indirect]", FSRC_PRIM, proc, argc, MZ_RUNSTACK, self);
  else
    return proc(argc, MZ_RUNSTACK, self);
}

/* Various specific 'futurized' versions of primitives that may 
   be invoked directly from JIT code and are not considered thread-safe 
   (are not invoked via apply_multi_from_native, etc.) */

Scheme_Object *scheme_ts_scheme_force_value_same_mark(Scheme_Object *v)
{
  return ts_scheme_force_value_same_mark(v);
}

#endif

static Scheme_Object *_scheme_tail_apply_from_native_fixup_args(Scheme_Object *rator,
                                                                int argc,
                                                                Scheme_Object **argv)
{
  int already = fixup_already_in_place, i;
  Scheme_Object **base;

  base = fixup_runstack_base XFORM_OK_MINUS argc XFORM_OK_MINUS already;

  /* Need to shift argc to end of base: */
  for (i = 0; i < argc; i++) {
    base[already + i] = argv[i];
  }

  return ts__scheme_tail_apply_from_native(rator, argc + already, base);
}

static int generate_pause_for_gc_and_retry(mz_jit_state *jitter,
                                           int in_short_jumps,
                                           int gc_reg, /* must not be JIT_R1 */
                                           GC_CAN_IGNORE jit_insn *refagain)
{
#ifdef MZ_USE_FUTURES
  GC_CAN_IGNORE jit_insn *refslow = 0, *refpause;
  int i;

  mz_rs_sync();

  /* expose gc_reg to GC */
  mz_tl_sti_p(tl_jit_future_storage, gc_reg, JIT_R1);

  /* Save non-preserved registers. Use a multiple of 4 to avoid
     alignment problems. */
  jit_pushr_l(JIT_R1);
  jit_pushr_l(JIT_R2);
  jit_pushr_l(JIT_R0);
  jit_pushr_l(JIT_R0);
  CHECK_LIMIT();

  mz_tl_ldi_i(JIT_R0, tl_scheme_future_need_gc_pause);
  refpause = jit_bgti_i(jit_forward(), JIT_R0, 0);
  
  for (i = 0; i < 2; i++) {
    /* Restore non-preserved registers, and also move the gc-exposed
       register back. */
    if (i == 1) {
      mz_patch_branch(refpause);
      JIT_UPDATE_THREAD_RSPTR();
      jit_prepare(0);
      mz_finish(scheme_future_gc_pause);
    }
    jit_popr_l(JIT_R0);
    jit_popr_l(JIT_R0);
    jit_popr_l(JIT_R2);
    CHECK_LIMIT();
    mz_tl_ldi_p(gc_reg, tl_jit_future_storage);
    jit_movi_p(JIT_R1, NULL);
    mz_tl_sti_p(tl_jit_future_storage, JIT_R1, JIT_R2);
    jit_popr_l(JIT_R1);
    CHECK_LIMIT();
    if (!i)
      refslow = jit_jmpi(jit_forward());
    else
      (void)jit_jmpi(refagain);
  }

  mz_patch_ucbranch(refslow);
  
  return 1;
#else
  return 1;
#endif
}

static int generate_direct_prim_tail_call(mz_jit_state *jitter, int num_rands)
{
  /* JIT_V1 must have the target function pointer.
     Also, scheme_current_runstack must be up-to-date...
     unless num-rands == 1, in which case JIT_R0 must
     have the argument. */
  if (num_rands == 1) {
    jit_subi_p(JIT_RUNSTACK, JIT_RUNSTACK, WORDS_TO_BYTES(1));
    CHECK_RUNSTACK_OVERFLOW();
    jit_str_p(JIT_RUNSTACK, JIT_R0);
    JIT_UPDATE_THREAD_RSPTR();
  }
  jit_movi_i(JIT_R1, num_rands);
  mz_prepare_direct_prim(2); /* a prim takes 3 args, but a NONCM prim ignores the 3rd */
  CHECK_LIMIT();
  {
    /* May use JIT_R0 and create local branch: */
    mz_generate_direct_prim(jit_pusharg_p(JIT_RUNSTACK),
                            jit_pusharg_i(JIT_R1),
                            JIT_V1, scheme_noncm_prim_indirect);
  }
  CHECK_LIMIT();
  /*  Return: */
  mz_pop_threadlocal();
  mz_pop_locals();
  jit_ret();

  return 1;
}

int scheme_generate_tail_call(mz_jit_state *jitter, int num_rands, int direct_native, int need_set_rs, int is_inline)
/* Proc is in V1, args are at RUNSTACK.
   If num_rands < 0, then argc is in LOCAL2 and arguments are already below RUNSTACK_BASE.
   If direct_native == 2, then some arguments are already in place (shallower in the runstack
   than the arguments to move). */
{
  int i;
  GC_CAN_IGNORE jit_insn *refagain, *ref, *ref2, *ref4, *ref5;

  __START_SHORT_JUMPS__(num_rands < 100);

  /* First, try fast direct jump to native code: */
  if (!direct_native) {
    ref = jit_bmsi_ul(jit_forward(), JIT_V1, 0x1);
    jit_ldr_s(JIT_R1, JIT_V1);
    ref2 = jit_bnei_i(jit_forward(), JIT_R1, scheme_native_closure_type);
    CHECK_LIMIT();
  } else {
    ref = ref2 = NULL;
  }

  refagain = _jit.x.pc;

  /* Right kind of function. Extract data and check stack depth: */
  jit_ldxi_p(JIT_R0, JIT_V1, &((Scheme_Native_Closure *)0x0)->code);
  jit_ldxi_i(JIT_R2, JIT_R0, &((Scheme_Native_Closure_Data *)0x0)->max_let_depth);
  mz_tl_ldi_p(JIT_R1, tl_MZ_RUNSTACK_START);
  jit_subr_ul(JIT_R1, JIT_RUNSTACK, JIT_R1);
  ref4 = jit_bltr_ul(jit_forward(), JIT_R1, JIT_R2);
  CHECK_LIMIT();

  /* Fast jump ok (proc will check argc).
     At this point, V1 = closure and R0 = code. */

  /* Check for thread swap: */
  (void)mz_tl_ldi_i(JIT_R2, tl_scheme_fuel_counter);
  ref5 = jit_blei_i(jit_forward(), JIT_R2, 0);
#ifndef FUEL_AUTODECEREMENTS
  jit_subi_p(JIT_R2, JIT_R2, 0x1);
  (void)mz_tl_sti_i(tl_scheme_fuel_counter, JIT_R2, JIT_R1);
#endif
  CHECK_LIMIT();

  /* Copy args to runstack base: */
  if (num_rands >= 0) {
    /* Fixed argc: */
    if (num_rands) {
      mz_ld_runstack_base_alt(JIT_R2);
      jit_subi_p(JIT_R2, JIT_RUNSTACK_BASE_OR_ALT(JIT_R2), WORDS_TO_BYTES(num_rands)); 
      CHECK_RUNSTACK_OVERFLOW();
      for (i = num_rands; i--; ) {
        jit_ldxi_p(JIT_R1, JIT_RUNSTACK, WORDS_TO_BYTES(i));
        jit_stxi_p(WORDS_TO_BYTES(i), JIT_R2, JIT_R1);
        CHECK_LIMIT();
      }
      jit_movr_p(JIT_RUNSTACK, JIT_R2);
    } else {
#ifdef JIT_RUNSTACK_BASE
      jit_movr_p(JIT_RUNSTACK, JIT_RUNSTACK_BASE);
#else
      mz_get_local_p(JIT_RUNSTACK, JIT_RUNSTACK_BASE_LOCAL);
#endif
    }
    if (direct_native > 1) { /* => some_args_already_in_place */
      mz_get_local_p(JIT_R1, JIT_LOCAL2);
      jit_lshi_l(JIT_R1, JIT_R1, JIT_LOG_WORD_SIZE);
      jit_subr_p(JIT_RUNSTACK, JIT_RUNSTACK, JIT_R1);
      CHECK_RUNSTACK_OVERFLOW();
    }
  } else {
    /* Variable argc (in LOCAL2):
       arguments are already in place. */
  }
  /* RUNSTACK, RUNSTACK_BASE, V1, and R0 are ready */
  
  /* Extract function and data: */
  jit_movr_p(JIT_R2, JIT_V1);
  if (direct_native) {
    jit_ldxi_p(JIT_V1, JIT_R0, &((Scheme_Native_Closure_Data *)0x0)->u.tail_code);
  } else {
    jit_ldxi_p(JIT_V1, JIT_R0, &((Scheme_Native_Closure_Data *)0x0)->arity_code);
  }
  /* Set up arguments; JIT_RUNSTACK and JIT_RUNSTACK_BASE must also be ready */
  jit_movr_p(JIT_R0, JIT_R2);
  if (num_rands >= 0) {
    jit_movi_i(JIT_R1, num_rands);
    if (direct_native > 1) { /* => some_args_already_in_place */
      mz_get_local_p(JIT_R2, JIT_LOCAL2);
      jit_addr_i(JIT_R1, JIT_R1, JIT_R2);
    }
  } else {
    mz_get_local_p(JIT_R1, JIT_LOCAL2);    
  }
  jit_movr_p(JIT_R2, JIT_RUNSTACK);
  if (need_set_rs) {
    /* In case arity check fails, need to update runstack now: */
    JIT_UPDATE_THREAD_RSPTR();
  }
  /* Now jump: */
  jit_jmpr(JIT_V1);
  CHECK_LIMIT();

  if (!direct_native && !is_inline && (num_rands >= 0)) {
    /* Handle simple applicable struct: */
    mz_patch_branch(ref2);
    ref2 = generate_proc_struct_retry(jitter, num_rands, refagain);
    CHECK_LIMIT();
  }

  /* The slow way: */
  /*  V1 and RUNSTACK must be intact! */
  mz_patch_branch(ref5);
  generate_pause_for_gc_and_retry(jitter,
                                  num_rands < 100,  /* in short jumps */
                                  JIT_V1, /* expose V1 to GC */
                                  refagain); /* retry code pointer */
  CHECK_LIMIT();
  if (!direct_native) {
    mz_patch_branch(ref);
    mz_patch_branch(ref2);
  }
  mz_patch_branch(ref4);
  CHECK_LIMIT();
  if (need_set_rs) {
    JIT_UPDATE_THREAD_RSPTR();
  }
  if (direct_native > 1) { /* => some_args_already_in_place */
    /* Need to shuffle argument lists. Since we can pass only
       three arguments, use static variables for the others. */
    mz_ld_runstack_base_alt(JIT_R1);
    mz_tl_sti_p(tl_fixup_runstack_base, JIT_RUNSTACK_BASE_OR_ALT(JIT_R1), JIT_R0);
    mz_get_local_p(JIT_R1, JIT_LOCAL2);
    mz_tl_sti_l(tl_fixup_already_in_place, JIT_R1, JIT_R0);
  }
  if (num_rands >= 0) {
    jit_movi_i(JIT_R0, num_rands);
  } else {
    mz_get_local_p(JIT_R0, JIT_LOCAL2);    
  }
  /* Since we've overwritten JIT_RUNSTACK, if this is not shared
     code, and if this is 3m, then the runstack no longer
     has a pointer to the closure for this code. To ensure that
     an appropriate return point exists, jump to static code
     for the rest. (This is the slow path, anyway.) */
  __END_SHORT_JUMPS__(num_rands < 100);
  if (direct_native > 1) {
    (void)jit_jmpi(sjc.finish_tail_call_fixup_code);
  } else {
    (void)jit_jmpi(sjc.finish_tail_call_code);
  }
  
  return 1;
}

int scheme_generate_finish_tail_call(mz_jit_state *jitter, int direct_native)
{
  mz_prepare(3);
  CHECK_LIMIT();
  jit_pusharg_p(JIT_RUNSTACK);
  jit_pusharg_i(JIT_R0);
  jit_pusharg_p(JIT_V1);
  if (direct_native > 1) { /* => some_args_already_in_place */
    (void)mz_finish(_scheme_tail_apply_from_native_fixup_args);
  } else {
    GC_CAN_IGNORE jit_insn *refr;
    (void)mz_finish_lwe(ts__scheme_tail_apply_from_native, refr);
  }
  CHECK_LIMIT();
  /* Return: */
  mz_pop_threadlocal();
  mz_pop_locals();
  jit_ret();

  return 1;
}

static int generate_direct_prim_non_tail_call(mz_jit_state *jitter, int num_rands, int multi_ok, int pop_and_jump)
{
  /* See generate_prim_non_tail_call for assumptions. */

  if (pop_and_jump) {
    mz_prolog(JIT_R1);
  }

  if (num_rands == 1) {
    jit_subi_p(JIT_RUNSTACK, JIT_RUNSTACK, WORDS_TO_BYTES(1));
    CHECK_RUNSTACK_OVERFLOW();
    jit_str_p(JIT_RUNSTACK, JIT_R0);
    JIT_UPDATE_THREAD_RSPTR();
  }

  jit_movi_i(JIT_R1, num_rands);
  mz_prepare_direct_prim(2); /* a prim takes 3 args, but a NONCM prim ignores the 3rd */
  CHECK_LIMIT();
  {
    /* May use JIT_R0 and create local branch: */
    mz_generate_direct_prim(jit_pusharg_p(JIT_RUNSTACK),
                            jit_pusharg_i(JIT_R1),
                            JIT_V1, scheme_noncm_prim_indirect);
  }
  CHECK_LIMIT();
  jit_retval(JIT_R0);
  VALIDATE_RESULT(JIT_R0);
  /* No need to check for multi values or tail-call, because
     we only use this for noncm primitives. */

  if (num_rands == 1) {
    jit_addi_p(JIT_RUNSTACK, JIT_RUNSTACK, WORDS_TO_BYTES(1));
    jitter->need_set_rs = 1;
  }

  if (pop_and_jump) {
    mz_epilog(JIT_V1);
  }

  return 1;
}

static int generate_retry_call(mz_jit_state *jitter, int num_rands, int multi_ok, GC_CAN_IGNORE jit_insn *reftop)
  /* If num_rands < 0, original argc is in V1, and we should
     pop argc arguments off runstack before pushing more.
     This function is called with short jumps enabled. */
{
  GC_CAN_IGNORE jit_insn *ref, *ref2, *refloop;

  if (!reftop) {
    reftop = sjc.shared_non_tail_retry_code[multi_ok ? 1 : 0];
  }

  /* Get new argc: */
  (void)mz_tl_ldi_p(JIT_R1, tl_scheme_current_thread);
  jit_ldxi_l(JIT_R2, JIT_R1, &((Scheme_Thread *)0x0)->ku.apply.tail_num_rands);
  if (num_rands >= 0) {
    jit_movi_l(JIT_V1, 0);
  }
  /* Thread is in R1. New argc is in R2. Old argc to cancel is in V1. */

  /* Enough room on runstack? */
  mz_tl_ldi_p(JIT_R0, tl_MZ_RUNSTACK_START);
  jit_subr_ul(JIT_R0, JIT_RUNSTACK, JIT_R0); /* R0 is space left (in bytes) */
  jit_subr_l(JIT_R2, JIT_R2, JIT_V1);
  jit_lshi_l(JIT_R2, JIT_R2, JIT_LOG_WORD_SIZE);
  ref = jit_bltr_ul(jit_forward(), JIT_R0, JIT_R2);
  CHECK_LIMIT();

  /* Yes, there's enough room. Adjust the runstack. */
  jit_subr_l(JIT_RUNSTACK, JIT_RUNSTACK, JIT_R2);
  CHECK_RUNSTACK_OVERFLOW();

  /* Copy arguments to runstack, then jump to reftop. */
  jit_ldxi_l(JIT_R2, JIT_R1, &((Scheme_Thread *)0x0)->ku.apply.tail_num_rands);
  jit_ldxi_l(JIT_V1, JIT_R1, &((Scheme_Thread *)0x0)->ku.apply.tail_rands);
  jit_lshi_l(JIT_R2, JIT_R2, JIT_LOG_WORD_SIZE);
  CHECK_LIMIT();
  refloop = _jit.x.pc;
  ref2 = jit_blei_l(jit_forward(), JIT_R2, 0);
  jit_subi_l(JIT_R2, JIT_R2, JIT_WORD_SIZE);
  jit_ldxr_p(JIT_R0, JIT_V1, JIT_R2);
  jit_stxr_p(JIT_R2, JIT_RUNSTACK, JIT_R0);
  (void)jit_jmpi(refloop);
  CHECK_LIMIT();

  /* R1 is still the thread. 
     Put procedure and argc in place, then jump to apply: */
  mz_patch_branch(ref2);
  jit_ldxi_l(JIT_V1, JIT_R1, &((Scheme_Thread *)0x0)->ku.apply.tail_rator);
  jit_ldxi_l(JIT_R0, JIT_R1, &((Scheme_Thread *)0x0)->ku.apply.tail_num_rands);
  __END_SHORT_JUMPS__(1);
  (void)jit_jmpi(reftop);
  __START_SHORT_JUMPS__(1);
  
  /* Slow path; restore R0 to SCHEME_TAIL_CALL_WAITING */
  mz_patch_branch(ref);
  jit_movi_l(JIT_R0, SCHEME_TAIL_CALL_WAITING);

  return 1;
}

static int generate_clear_previous_args(mz_jit_state *jitter, int num_rands)
{
  if (num_rands >= 0) {
    int i;
    for (i = 0; i < num_rands; i++) {
      jit_stxi_p(WORDS_TO_BYTES(i), JIT_RUNSTACK, JIT_RUNSTACK);
      CHECK_LIMIT();
    }
  } else {
    /* covered by generate_clear_slow_previous_args */
  }
  return 1;
}

static int generate_clear_slow_previous_args(mz_jit_state *jitter)
{
  CHECK_LIMIT();
  mz_prepare(3);
  jit_pusharg_p(JIT_R0);
  jit_pusharg_l(JIT_V1);
  jit_pusharg_l(JIT_RUNSTACK);
  (void)mz_finish(clear_runstack);
  jit_retval(JIT_R0);
  return 1;
}

int scheme_generate_non_tail_call(mz_jit_state *jitter, int num_rands, int direct_native, int need_set_rs, 
				  int multi_ok, int nontail_self, int pop_and_jump, int is_inlined)
{
  /* Non-tail call.
     Proc is in V1, args are at RUNSTACK.
     If nontail_self, then R0 has proc pointer, and R2 has max_let_depth.
     If num_rands < 0, then argc is in R0, and need to pop runstack before returning.
     If num_rands == -1, skip prolog. */
  GC_CAN_IGNORE jit_insn *ref, *ref2, *ref4, *ref5, *ref6, *ref7, *ref8, *ref9;
  GC_CAN_IGNORE jit_insn *ref10, *reftop = NULL, *refagain, *refrts;
#ifndef FUEL_AUTODECEREMENTS
  GC_CAN_IGNORE jit_insn *ref11;
#endif

  __START_SHORT_JUMPS__(1);

  if (pop_and_jump) {
    if (num_rands != -1) {
      mz_prolog(JIT_R1);
    } else {
      reftop = _jit.x.pc;
    }
  }

  /* Check for inlined native type */
  if (!direct_native) {
    ref = jit_bmsi_ul(jit_forward(), JIT_V1, 0x1);
    jit_ldr_s(JIT_R1, JIT_V1);
    ref2 = jit_bnei_i(jit_forward(), JIT_R1, scheme_native_closure_type);
    CHECK_LIMIT();
  } else {
    ref = ref2 = NULL;
  }

  refagain = _jit.x.pc;
      
  /* Before inlined native, check max let depth */
  if (!nontail_self) {
    jit_ldxi_p(JIT_R2, JIT_V1, &((Scheme_Native_Closure *)0x0)->code);
    jit_ldxi_i(JIT_R2, JIT_R2, &((Scheme_Native_Closure_Data *)0x0)->max_let_depth);
  }
  mz_tl_ldi_p(JIT_R1, tl_MZ_RUNSTACK_START);
  jit_subr_ul(JIT_R1, JIT_RUNSTACK, JIT_R1);
  ref4 = jit_bltr_ul(jit_forward(), JIT_R1, JIT_R2);
  CHECK_LIMIT();

  /* Before inlined native, check stack depth: */
  (void)mz_tl_ldi_p(JIT_R1, tl_scheme_jit_stack_boundary); /* assumes USE_STACK_BOUNDARY_VAR */
  ref9 = jit_bltr_ul(jit_forward(), JIT_SP, JIT_R1); /* assumes down-growing stack */
  CHECK_LIMIT();

#ifndef FUEL_AUTODECEREMENTS
  /* Finally, check for thread swap: */
  (void)mz_tl_ldi_i(JIT_R2, tl_scheme_fuel_counter);
  ref11 = jit_blei_i(jit_forward(), JIT_R2, 0);
  jit_subi_p(JIT_R2, JIT_R2, 0x1);
  (void)mz_tl_sti_i(tl_scheme_fuel_counter, JIT_R2, JIT_R1);
#endif

  /* Fast inlined-native jump ok (proc will check argc, if necessary) */
  {
    GC_CAN_IGNORE jit_insn *refr;
    if (num_rands < 0) {
      /* We need to save argc to manually pop the
         runstack. So move V1 to R2 and move R0 to V1: */
      jit_movr_p(JIT_R2, JIT_V1);
      jit_movr_p(JIT_V1, JIT_R0);
    }
    refr = jit_patchable_movi_p(JIT_R1, jit_forward());
    jit_shuffle_saved_regs(); /* maybe copies V registers to be restored */
    _jit_prolog_again(jitter, 3, JIT_R1); /* saves V registers (or copied V registers) */
    if (num_rands >= 0) {
      if (nontail_self) { jit_movr_p(JIT_R1, JIT_R0); }
      jit_movr_p(JIT_R0, JIT_V1); /* closure */
      if (!nontail_self) {
        /* nontail_self is only enabled when there are no rest args: */
        jit_movi_i(JIT_R1, num_rands); /* argc */
        jit_movr_p(JIT_R2, JIT_RUNSTACK); /* argv */
      }
      jit_addi_p(JIT_RUNSTACK_BASE_OR_ALT(JIT_V1), JIT_RUNSTACK, WORDS_TO_BYTES(num_rands));
      mz_st_runstack_base_alt(JIT_V1);
    } else {
      /* R2 is closure, V1 is argc */
      jit_lshi_l(JIT_R1, JIT_V1, JIT_LOG_WORD_SIZE);
      jit_addr_p(JIT_RUNSTACK_BASE_OR_ALT(JIT_R0), JIT_RUNSTACK, JIT_R1);
      mz_st_runstack_base_alt(JIT_R0);
      jit_movr_p(JIT_R0, JIT_R2); /* closure */
      jit_movr_i(JIT_R1, JIT_V1); /* argc */
      jit_movr_p(JIT_R2, JIT_RUNSTACK); /* argv */
    }
    CHECK_LIMIT();
    mz_push_locals();
    mz_repush_threadlocal();
    if (!nontail_self) {
      jit_ldxi_p(JIT_V1, JIT_R0, &((Scheme_Native_Closure *)0x0)->code);
      if (direct_native) {
        jit_ldxi_p(JIT_V1, JIT_V1, &((Scheme_Native_Closure_Data *)0x0)->u.tail_code);
      } else {
        jit_ldxi_p(JIT_V1, JIT_V1, &((Scheme_Native_Closure_Data *)0x0)->arity_code);
        if (need_set_rs) {
          /* In case arity check fails, need to update runstack now: */
          JIT_UPDATE_THREAD_RSPTR();
        }
      }
      jit_jmpr(JIT_V1); /* callee restores (copied) V registers, etc. */
    } else {
      /* self-call function pointer is in R1 */
      jit_jmpr(JIT_R1);
    }
    jit_patch_movi(refr, (_jit.x.pc));
    jit_unshuffle_saved_regs(); /* maybe uncopies V registers */
    /* If num_rands < 0, then V1 has argc */
  }
  CHECK_LIMIT();
  jit_retval(JIT_R0);
  VALIDATE_RESULT(JIT_R0);

  /* Fast common-case return */
  if (pop_and_jump) {
    GC_CAN_IGNORE jit_insn *refc;
    __START_INNER_TINY__(1);
    refc = jit_blei_p(jit_forward(), JIT_R0, SCHEME_MULTIPLE_VALUES);
    __END_INNER_TINY__(1);
    if (num_rands < 0) { 
      /* At this point, argc must be in V1 */
      jit_lshi_l(JIT_R1, JIT_V1, JIT_LOG_WORD_SIZE);
      jit_addr_p(JIT_RUNSTACK, JIT_RUNSTACK, JIT_R1);
    }
    if (pop_and_jump) {
      mz_epilog(JIT_V1);
    }
    __START_INNER_TINY__(1);
    mz_patch_branch(refc);
    __END_INNER_TINY__(1);
    CHECK_LIMIT();
  }

  if (!multi_ok) {
    GC_CAN_IGNORE jit_insn *refm;
    __END_SHORT_JUMPS__(1);
    refm = jit_beqi_p(jit_forward(), JIT_R0, SCHEME_MULTIPLE_VALUES);
    mz_patch_branch_at(refm, sjc.bad_result_arity_code);
    __START_SHORT_JUMPS__(1);
  }
  ref6 = jit_bnei_p(jit_forward(), JIT_R0, SCHEME_TAIL_CALL_WAITING);
  generate_clear_previous_args(jitter, num_rands);
  CHECK_LIMIT();
  if (pop_and_jump) {
    /* Expects argc in V1 if num_rands < 0: */
    generate_retry_call(jitter, num_rands, multi_ok, reftop);
  }
  CHECK_LIMIT();
  if (need_set_rs) {
    JIT_UPDATE_THREAD_RSPTR();
  }
  if (num_rands < 0) {
    generate_clear_slow_previous_args(jitter);
    CHECK_LIMIT();
  }
  mz_prepare(1);
  jit_pusharg_p(JIT_R0);
  if (multi_ok) {
    (void)mz_finish_lwe(ts_scheme_force_value_same_mark, refrts);
  } else {
    (void)mz_finish_lwe(ts_scheme_force_one_value_same_mark, refrts);
  }
  ref5 = jit_jmpi(jit_forward());
  CHECK_LIMIT();

  /* Maybe it's a prim? */
  if (!direct_native) {
    mz_patch_branch(ref2);
    ref2 = jit_bnei_i(jit_forward(), JIT_R1, scheme_prim_type);
    /* It's a prim. Arity check... fast path when exactly equal to min, only: */
    jit_ldxi_i(JIT_R2, JIT_V1, &((Scheme_Primitive_Proc *)0x0)->mina);
    if (num_rands >= 0) {
      ref7 = jit_bnei_i(jit_forward(), JIT_R2, num_rands);
    } else {
      ref7 = jit_bner_i(jit_forward(), JIT_R2, JIT_R0);
    }
    /* Fast prim application */
    jit_ldxi_p(JIT_R1, JIT_V1, &((Scheme_Primitive_Proc *)0x0)->prim_val);
    if (need_set_rs) {
      JIT_UPDATE_THREAD_RSPTR();
    }
	mz_prepare_direct_prim(3);
    jit_pusharg_p(JIT_V1);
    CHECK_LIMIT();
    if (num_rands < 0) { jit_movr_p(JIT_V1, JIT_R0); } /* save argc to manually pop runstack */
    {
      __END_SHORT_JUMPS__(1);
      /* May use JIT_R0 and create local branch: */
      mz_generate_direct_prim(jit_pusharg_p(JIT_RUNSTACK),
                              jit_pusharg_i(JIT_R2),
                              JIT_R1, scheme_prim_indirect);
      __START_SHORT_JUMPS__(1);
    }
    CHECK_LIMIT();
    jit_retval(JIT_R0);
    VALIDATE_RESULT(JIT_R0);
    if (!multi_ok) {
      GC_CAN_IGNORE jit_insn *refm;
      __END_SHORT_JUMPS__(1);
      refm = jit_beqi_p(jit_forward(), JIT_R0, SCHEME_MULTIPLE_VALUES);
      mz_patch_branch_at(refm, sjc.bad_result_arity_code);
      __START_SHORT_JUMPS__(1);
    }
    ref10 = jit_bnei_p(jit_forward(), JIT_R0, SCHEME_TAIL_CALL_WAITING);
    generate_clear_previous_args(jitter, num_rands);
    CHECK_LIMIT();
    if (pop_and_jump) {
      /* Expects argc in V1 if num_rands < 0: */
      generate_retry_call(jitter, num_rands, multi_ok, reftop);
    }
    CHECK_LIMIT();
    if (num_rands < 0) {
      generate_clear_slow_previous_args(jitter);
      CHECK_LIMIT();
    }
    mz_prepare(1);
    jit_pusharg_p(JIT_R0);
    if (multi_ok) {
      (void)mz_finish_lwe(ts_scheme_force_value_same_mark, refrts);
    } else {
      (void)mz_finish_lwe(ts_scheme_force_one_value_same_mark, refrts);
    }
    CHECK_LIMIT();
    ref8 = jit_jmpi(jit_forward());

    /* Check for simple applicable struct wrapper */
    if (!is_inlined && (num_rands >= 0)) {
      mz_patch_branch(ref2);
      ref2 = generate_proc_struct_retry(jitter, num_rands, refagain);
      CHECK_LIMIT();
    }
  } else {
    ref2 = ref7 = ref8 = ref10 = NULL;
  }

  /* The slow way: */
  mz_patch_branch(ref9);
  generate_pause_for_gc_and_retry(jitter,
                                  1,  /* in short jumps */
                                  JIT_V1, /* expose V1 to GC */
                                  refagain); /* retry code pointer */
  CHECK_LIMIT();
  if (!direct_native) {
    mz_patch_branch(ref);
    mz_patch_branch(ref2);
    mz_patch_branch(ref7);
  }
  mz_patch_branch(ref4);  
#ifndef FUEL_AUTODECEREMENTS
  mz_patch_branch(ref11);
#endif
  if (need_set_rs) {
    JIT_UPDATE_THREAD_RSPTR();
  }
  if (num_rands >= 0) {
    jit_movi_i(JIT_R0, num_rands);
  }
  mz_prepare(3);
  CHECK_LIMIT();
  jit_pusharg_p(JIT_RUNSTACK);
  jit_pusharg_i(JIT_R0);
  jit_pusharg_p(JIT_V1);
  if (num_rands < 0) { jit_movr_p(JIT_V1, JIT_R0); } /* save argc to manually pop runstack */
  if (multi_ok) {
    (void)mz_finish_lwe(ts__scheme_apply_multi_from_native, refrts);
  } else {
    (void)mz_finish_lwe(ts__scheme_apply_from_native, refrts);
  }
  CHECK_LIMIT();
  mz_patch_ucbranch(ref5);
  if (!direct_native) {
    mz_patch_ucbranch(ref8);
  }
  jit_retval(JIT_R0);
  VALIDATE_RESULT(JIT_R0);
  mz_patch_branch(ref6);
  if (!direct_native) {
    mz_patch_branch(ref10);
  }
  /* Note: same return code is above for faster common-case return */
  if (num_rands < 0) { 
    /* At this point, argc must be in V1 */
    jit_lshi_l(JIT_R1, JIT_V1, JIT_LOG_WORD_SIZE);
    jit_addr_p(JIT_RUNSTACK, JIT_RUNSTACK, JIT_R1);
  }
  if (pop_and_jump) {
    mz_epilog(JIT_V1);
  }
  CHECK_LIMIT();

  __END_SHORT_JUMPS__(1);

  return 1;
}

static int generate_self_tail_call(Scheme_Object *rator, mz_jit_state *jitter, int num_rands, GC_CAN_IGNORE jit_insn *slow_code,
                                   int args_already_in_place, Scheme_App_Rec *app, Scheme_Object **alt_rands)
/* Last argument is in R0 */
{
  GC_CAN_IGNORE jit_insn *refslow, *refagain;
  int i, jmp_tiny, jmp_short;
  int closure_size = jitter->self_closure_size;
  int space, offset, arg_offset, arg_tmp_offset;
#ifdef USE_FLONUM_UNBOXING
  Scheme_Object *rand;
#endif

#ifdef JIT_PRECISE_GC
  closure_size += 1; /* Skip procedure pointer, too */
#endif

  jmp_tiny = num_rands < 5;
  jmp_short = num_rands < 100;

  __START_TINY_OR_SHORT_JUMPS__(jmp_tiny, jmp_short);

  refagain = _jit.x.pc;

  /* Check for thread swap: */
  (void)mz_tl_ldi_i(JIT_R2, tl_scheme_fuel_counter);
  refslow = jit_blei_i(jit_forward(), JIT_R2, 0);
#ifndef FUEL_AUTODECEREMENTS
  jit_subi_p(JIT_R2, JIT_R2, 0x1);
  (void)mz_tl_sti_i(tl_scheme_fuel_counter, JIT_R2, JIT_R1);
#endif

  __END_TINY_OR_SHORT_JUMPS__(jmp_tiny, jmp_short);

  arg_tmp_offset = offset = jitter->flostack_offset;
  space = jitter->flostack_space;

  arg_offset = 1;

  /* Copy args to runstack after closure data: */
  mz_ld_runstack_base_alt(JIT_R2);
  jit_subi_p(JIT_R2, JIT_RUNSTACK_BASE_OR_ALT(JIT_R2), WORDS_TO_BYTES(num_rands + closure_size + args_already_in_place)); 
  for (i = num_rands; i--; ) {
    int already_loaded = (i == num_rands - 1);
#ifdef USE_FLONUM_UNBOXING
    int is_flonum, already_unboxed = 0;
    if ((SCHEME_CLOSURE_DATA_FLAGS(jitter->self_data) & CLOS_HAS_TYPED_ARGS)
        && CLOSURE_ARGUMENT_IS_FLONUM(jitter->self_data, i + args_already_in_place)) {
      int aoffset;
      is_flonum = 1;
      rand = (alt_rands 
              ? alt_rands[i+1+args_already_in_place] 
              : app->args[i+1+args_already_in_place]);
      aoffset = JIT_FRAME_FLONUM_OFFSET - (arg_tmp_offset * sizeof(double));
      jit_ldxi_d_fppush(JIT_FPR0, JIT_FP, aoffset);
      --arg_tmp_offset;
      already_unboxed = 1;
      if (!already_loaded && !SAME_TYPE(SCHEME_TYPE(rand), scheme_local_type)) {
        already_loaded = 1;
        (void)jit_movi_p(JIT_R0, NULL);
      }
    } else
      is_flonum = 0;
#endif
    if (!already_loaded)
      jit_ldxi_p(JIT_R0, JIT_RUNSTACK, WORDS_TO_BYTES(i));
    jit_stxi_p(WORDS_TO_BYTES(i + closure_size + args_already_in_place), JIT_R2, JIT_R0);
#ifdef USE_FLONUM_UNBOXING
    if (is_flonum) {
      int aoffset;
      if (!already_unboxed)
        jit_ldxi_d_fppush(JIT_FPR0, JIT_R0, &((Scheme_Double *)0x0)->double_val); 
      aoffset = JIT_FRAME_FLONUM_OFFSET - (arg_offset * sizeof(double));
      (void)jit_stxi_d_fppop(aoffset, JIT_FP, JIT_FPR0);
      arg_offset++;
    }
#endif
    CHECK_LIMIT();
  }
  jit_movr_p(JIT_RUNSTACK, JIT_R2);

  scheme_mz_flostack_restore(jitter, jitter->self_restart_space, jitter->self_restart_offset, 1, 1);

  /* Now jump: */
  (void)jit_jmpi(jitter->self_restart_code);
  CHECK_LIMIT();
  
  /* Slow path: */
  __START_TINY_OR_SHORT_JUMPS__(jmp_tiny, jmp_short);
  mz_patch_branch(refslow);
  __END_TINY_OR_SHORT_JUMPS__(jmp_tiny, jmp_short);

  generate_pause_for_gc_and_retry(jitter,
                                  0,  /* in short jumps */
                                  JIT_R0, /* expose R0 to GC */
                                  refagain); /* retry code pointer */
  CHECK_LIMIT();

  jitter->flostack_offset = offset;
  jitter->flostack_space = space;

#ifdef USE_FLONUM_UNBOXING
  /* Need to box any arguments that we have only in flonum form */
  if (SCHEME_CLOSURE_DATA_FLAGS(jitter->self_data) & CLOS_HAS_TYPED_ARGS) {
    arg_tmp_offset = offset;
    for (i = num_rands; i--; ) {
      if (CLOSURE_ARGUMENT_IS_FLONUM(jitter->self_data, i + args_already_in_place)) {
        rand = (alt_rands 
                ? alt_rands[i+1+args_already_in_place] 
                : app->args[i+1+args_already_in_place]);
        if (!SAME_TYPE(SCHEME_TYPE(rand), scheme_local_type)
            || (SCHEME_GET_LOCAL_FLAGS(rand) == SCHEME_LOCAL_FLONUM)) {
          int aoffset = JIT_FRAME_FLONUM_OFFSET - (arg_tmp_offset * sizeof(double));
          GC_CAN_IGNORE jit_insn *iref;
          if (i != num_rands - 1)
            mz_pushr_p(JIT_R0);
          if (SAME_TYPE(SCHEME_TYPE(rand), scheme_local_type)) {
            /* assert: SCHEME_GET_LOCAL_FLAGS(rand) == SCHEME_LOCAL_FLONUM */
            /* have to check for an existing box */
            if (i != num_rands - 1)
              mz_rs_ldxi(JIT_R0, i+1);
            mz_rs_sync();
            __START_TINY_JUMPS__(1);
            iref = jit_bnei_p(jit_forward(), JIT_R0, NULL);
            __END_TINY_JUMPS__(1);
          } else
            iref = NULL;
          jit_movi_l(JIT_R0, aoffset);
          mz_rs_sync();
          (void)jit_calli(sjc.box_flonum_from_stack_code);
          if (i != num_rands - 1)
            mz_rs_stxi(i+1, JIT_R0);
          if (iref) {
            __START_TINY_JUMPS__(1);
            mz_patch_branch(iref);
            __END_TINY_JUMPS__(1);
          }
          CHECK_LIMIT();
          if (i != num_rands - 1)
            mz_popr_p(JIT_R0);
          --arg_tmp_offset;
        }
      }
    }

    /* Arguments already in place may also need to be boxed. */
    arg_tmp_offset = jitter->self_restart_offset;
    for (i = 0; i < args_already_in_place; i++) {
      if (CLOSURE_ARGUMENT_IS_FLONUM(jitter->self_data, i)) {
        GC_CAN_IGNORE jit_insn *iref;
        mz_pushr_p(JIT_R0);
        mz_ld_runstack_base_alt(JIT_R2);
        jit_subi_p(JIT_R2, JIT_RUNSTACK_BASE_OR_ALT(JIT_R2), WORDS_TO_BYTES(num_rands + closure_size + args_already_in_place)); 
        jit_ldxi_p(JIT_R0, JIT_R2, WORDS_TO_BYTES(i+closure_size));
        mz_rs_sync();
        __START_TINY_JUMPS__(1);
        iref = jit_bnei_p(jit_forward(), JIT_R0, NULL);
        __END_TINY_JUMPS__(1);
        {
          int aoffset;
          aoffset = JIT_FRAME_FLONUM_OFFSET - (arg_tmp_offset * sizeof(double));
          jit_ldxi_d_fppush(JIT_FPR0, JIT_FP, aoffset);
          (void)jit_calli(sjc.box_flonum_from_stack_code);
          mz_ld_runstack_base_alt(JIT_R2);
          jit_subi_p(JIT_R2, JIT_RUNSTACK_BASE_OR_ALT(JIT_R2), WORDS_TO_BYTES(num_rands + closure_size + args_already_in_place)); 
          jit_stxi_p(WORDS_TO_BYTES(i+closure_size), JIT_R2, JIT_R0);
        }
        __START_TINY_JUMPS__(1);
        mz_patch_branch(iref);
        __END_TINY_JUMPS__(1);
        mz_popr_p(JIT_R0);
        CHECK_LIMIT();
        --arg_tmp_offset;
      }
    }
  }
#endif

  scheme_mz_flostack_restore(jitter, 0, 0, 1, 1);

  CHECK_LIMIT();

  if (args_already_in_place) {
    jit_movi_l(JIT_R2, args_already_in_place);
    mz_set_local_p(JIT_R2, JIT_LOCAL2);
  }

  mz_rs_stxi(num_rands - 1, JIT_R0);
  scheme_generate(rator, jitter, 0, 0, 0, JIT_V1, NULL);
  CHECK_LIMIT();
  mz_rs_sync();

  (void)jit_jmpi(slow_code);

  return 1;
}

typedef struct {
  int num_rands;
  mz_jit_state *old_jitter;
  int multi_ok;
  int is_tail;
  int direct_prim, direct_native, nontail_self;
} Generate_Call_Data;

void scheme_jit_register_sub_func(mz_jit_state *jitter, void *code, Scheme_Object *protocol)
/* protocol: #f => normal lightweight call protocol
             void => next return address is in LOCAL2
             eof => name to use is in LOCAL2 */
{
  void *code_end;

  code_end = jit_get_ip().ptr;
  if (jitter->retain_start)
    scheme_jit_add_symbol((uintptr_t)code, (uintptr_t)code_end - 1, protocol, 0);
}

void scheme_jit_register_helper_func(mz_jit_state *jitter, void *code)
{
#ifdef MZ_USE_DWARF_LIBUNWIND
  /* Null indicates that there's no function name to report, but the
     stack should be unwound manually using the JJIT-generated convention. */
  scheme_jit_register_sub_func(jitter, code, scheme_null);
#endif  
}

static int do_generate_shared_call(mz_jit_state *jitter, void *_data)
{
  Generate_Call_Data *data = (Generate_Call_Data *)_data;
  
#ifdef MZ_USE_JIT_PPC
  jitter->js.jitl.nbArgs = data->old_jitter->js.jitl.nbArgs;
#endif

  if (data->is_tail) {
    int ok;
    void *code;

    code = jit_get_ip().ptr;

    if (data->direct_prim)
      ok = generate_direct_prim_tail_call(jitter, data->num_rands);
    else
      ok = scheme_generate_tail_call(jitter, data->num_rands, data->direct_native, 1, 0);

    scheme_jit_register_helper_func(jitter, code);

    return ok;
  } else {
    int ok;
    void *code;

    code = jit_get_ip().ptr;

    if (data->direct_prim)
      ok = generate_direct_prim_non_tail_call(jitter, data->num_rands, data->multi_ok, 1);
    else
      ok = scheme_generate_non_tail_call(jitter, data->num_rands, data->direct_native, 1, 
                                         data->multi_ok, data->nontail_self, 1, 0);

    scheme_jit_register_sub_func(jitter, code, scheme_false);

    return ok;
  }
}

void *scheme_generate_shared_call(int num_rands, mz_jit_state *old_jitter, int multi_ok, int is_tail, 
				  int direct_prim, int direct_native, int nontail_self)
{
  Generate_Call_Data data;

  data.num_rands = num_rands;
  data.old_jitter = old_jitter;
  data.multi_ok = multi_ok;
  data.is_tail = is_tail;
  data.direct_prim = direct_prim;
  data.direct_native = direct_native;
  data.nontail_self = nontail_self;

  return scheme_generate_one(old_jitter, do_generate_shared_call, &data, 0, NULL, NULL);
}

void scheme_ensure_retry_available(mz_jit_state *jitter, int multi_ok)
{
  int mo = multi_ok ? 1 : 0;
  if (!sjc.shared_non_tail_retry_code[mo]) {
    void *code;
    code = scheme_generate_shared_call(-1, jitter, multi_ok, 0, 0, 0, 0);
    sjc.shared_non_tail_retry_code[mo] = code;
  }
}

static int generate_nontail_self_setup(mz_jit_state *jitter)
{
  void *pp, **pd;
  pp = jit_patchable_movi_p(JIT_R2, jit_forward());
  pd = (void **)scheme_malloc(2 * sizeof(void *));
  pd[0] = pp;
  pd[1] = jitter->patch_depth;
  jitter->patch_depth = pd;
  (void)jit_patchable_movi_p(JIT_R0, jitter->self_nontail_code);
#ifdef JIT_PRECISE_GC
  if (jitter->closure_self_on_runstack) {
    /* Get this closure's pointer from the run stack */
    int depth = jitter->depth + jitter->extra_pushed - 1;
    jit_ldxi_p(JIT_V1, JIT_RUNSTACK, WORDS_TO_BYTES(depth));
  }
#endif
  return 0;
}

static int can_direct_native(Scheme_Object *p, int num_rands, intptr_t *extract_case)
{
  if (SAME_TYPE(SCHEME_TYPE(p), scheme_native_closure_type)) {
    if (((Scheme_Native_Closure *)p)->code->closure_size < 0) {
      /* case-lambda */
      int cnt, i;
      mzshort *arities;

      cnt = ((Scheme_Native_Closure *)p)->code->closure_size;
      cnt = -(cnt + 1);
      arities = ((Scheme_Native_Closure *)p)->code->u.arities;
      for (i = 0; i < cnt; i++) {
        if (arities[i] == num_rands) {
          *extract_case = (intptr_t)&((Scheme_Native_Closure *)0x0)->vals[i];
          return 1;
        }
      }
    } else {
      /* not a case-lambda... */
      if (scheme_native_arity_check(p, num_rands)
          /* If it also accepts num_rands + 1, then it has a vararg,
             so don't try direct_native. */
          && !scheme_native_arity_check(p, num_rands + 1)) {
        return 1;
      }
    }
  }

  return 0;
}

int scheme_generate_app(Scheme_App_Rec *app, Scheme_Object **alt_rands, int num_rands, 
			mz_jit_state *jitter, int is_tail, int multi_ok, int no_call)
/* de-sync'd ok 
   If no_call is 2, then rator is not necssarily evaluated. 
   If no_call is 1, then rator is left in V1 and arguments are on runstack. */
{
  int i, offset, need_safety = 0, apply_to_list = 0;
  int direct_prim = 0, need_non_tail = 0, direct_native = 0, direct_self = 0, nontail_self = 0;
  int proc_already_in_place = 0;
  Scheme_Object *rator, *v, *arg;
  int reorder_ok = 0;
  int args_already_in_place = 0;
  intptr_t extract_case = 0; /* when direct_native, non-0 => offset to extract case-lambda case */
  START_JIT_DATA();

  rator = (alt_rands ? alt_rands[0] : app->args[0]);

  if (no_call == 2) {
    direct_prim = 1;
  } else if (SCHEME_PRIMP(rator)) {
    if ((num_rands >= ((Scheme_Primitive_Proc *)rator)->mina)
	&& ((num_rands <= ((Scheme_Primitive_Proc *)rator)->mu.maxa)
	    || (((Scheme_Primitive_Proc *)rator)->mina < 0))
	&& (scheme_is_noncm(rator, jitter, 0, 0)
            /* It's also ok to directly call `values' if multiple values are ok: */
            || (multi_ok && SAME_OBJ(rator, scheme_values_func))))
      direct_prim = 1;
    else {
      reorder_ok = 1;
      if ((num_rands >= 2) && SAME_OBJ(rator, scheme_apply_proc))
        apply_to_list = 1;
    }
  } else {
    Scheme_Type t;
    t = SCHEME_TYPE(rator);
    if ((t == scheme_local_type) && scheme_ok_to_delay_local(rator)) {
      /* We can re-order evaluation of the rator. */
      reorder_ok = 1;

      /* Call to known native, or even known self? */
      {
	int pos, flags;
	pos = SCHEME_LOCAL_POS(rator) - num_rands;
	if (scheme_mz_is_closure(jitter, pos, num_rands, &flags)) {
	  direct_native = 1;
	  if ((pos == jitter->self_pos)
	      && (num_rands < MAX_SHARED_CALL_RANDS)) {
            if (is_tail)
              direct_self = 1;
            else if (jitter->self_nontail_code)
              nontail_self = 1;
	  }
	}
      }
    } else if (t == scheme_toplevel_type) {
      if (SCHEME_TOPLEVEL_FLAGS(rator) & SCHEME_TOPLEVEL_CONST) {
        /* We can re-order evaluation of the rator. */
        reorder_ok = 1;

        if (jitter->nc) {
          Scheme_Object *p;

          p = scheme_extract_global(rator, jitter->nc);
          p = ((Scheme_Bucket *)p)->val;
          if (can_direct_native(p, num_rands, &extract_case)) {
            direct_native = 1;
            
            if ((SCHEME_TOPLEVEL_POS(rator) == jitter->self_toplevel_pos)
                && (num_rands < MAX_SHARED_CALL_RANDS)) {
              if (is_tail)
                direct_self = 1;
              else if (jitter->self_nontail_code)
                nontail_self = 1;
            }
          }
        }
      } else if (jitter->nc) {
        Scheme_Object *p;

        p = scheme_extract_global(rator, jitter->nc);
        if (((Scheme_Bucket_With_Flags *)p)->flags & GLOB_IS_CONSISTENT) {
          if (can_direct_native(((Scheme_Bucket *)p)->val, num_rands, &extract_case))
            direct_native = 1;
        }
      }
    } else if (SAME_TYPE(t, scheme_closure_type)) {
      Scheme_Closure_Data *data;
      data = ((Scheme_Closure *)rator)->code;
      if ((data->num_params == num_rands)
          && !(SCHEME_CLOSURE_DATA_FLAGS(data) & CLOS_HAS_REST)) {
        direct_native = 1;

        if (SAME_OBJ(data->u.jit_clone, jitter->self_data)
            && (num_rands < MAX_SHARED_CALL_RANDS)) {
          if (is_tail)
            direct_self = 1;
          else if (jitter->self_nontail_code)
            nontail_self = 1;
        }
      }
      reorder_ok = 1;
    } else if (t > _scheme_values_types_) {
      /* We can re-order evaluation of the rator. */
      reorder_ok = 1;
    }

#ifdef JIT_PRECISE_GC
    if (jitter->closure_self_on_runstack) {
      /* We can get this closure's pointer back from the Scheme stack. */
      if (nontail_self)
        direct_self = 1;
    }
#endif

    if (direct_self)
      reorder_ok = 0; /* superceded by direct_self */
  }

  /* Direct native tail with same number of args as just received? */
  if (direct_native && is_tail && num_rands
      && (num_rands == jitter->self_data->num_params)
      && !(SCHEME_CLOSURE_DATA_FLAGS(jitter->self_data) & CLOS_HAS_REST)) {
    /* Check whether the actual arguments refer to Scheme-stack 
       locations that will be filled with argument values; that
       is, check how many arguments are already in place for
       the call. */
    mz_runstack_skipped(jitter, num_rands);
    for (i = 0; i < num_rands; i++) {
      v = (alt_rands ? alt_rands[i+1] : app->args[i+1]);
      if (SAME_TYPE(SCHEME_TYPE(v), scheme_local_type)
          && !(SCHEME_GET_LOCAL_FLAGS(v) == SCHEME_LOCAL_OTHER_CLEARS)) {
        int pos;
        pos = mz_remap(SCHEME_LOCAL_POS(v));
        if (pos == (jitter->depth + jitter->extra_pushed + args_already_in_place))
          args_already_in_place++;
        else
          break;
      } else
        break;
    }
    mz_runstack_unskipped(jitter, num_rands);
    if (args_already_in_place) {
      direct_native = 2;
      mz_runstack_skipped(jitter, args_already_in_place);
      num_rands -= args_already_in_place;
    }
  }

  if (num_rands) {
    if (!direct_prim || (num_rands > 1) || (no_call == 2)) {
      mz_rs_dec(num_rands);
      need_safety = num_rands;
      CHECK_RUNSTACK_OVERFLOW();
      mz_runstack_pushed(jitter, num_rands);
    } else {
      mz_runstack_skipped(jitter, 1);
    }
  }

  for (i = num_rands + args_already_in_place + 1; i--; ) {
    v = (alt_rands ? alt_rands[i] : app->args[i]);
    if (!scheme_is_simple(v, INIT_SIMPLE_DEPTH, 1, jitter, 0)) {
      need_non_tail = 1;
      break;
    }
  }

  if (need_non_tail) {
    offset = scheme_generate_non_tail_mark_pos_prefix(jitter);
    CHECK_LIMIT();
  } else
    offset = 0;

  if (!direct_prim && !reorder_ok && !direct_self) {
    if (need_safety && !scheme_is_non_gc(rator, INIT_SIMPLE_DEPTH)) {
      scheme_stack_safety(jitter, need_safety, offset);
      CHECK_LIMIT();
      need_safety = 0;
    }

    scheme_generate_non_tail(rator, jitter, 0, !need_non_tail, 0); /* sync'd after args below */
    CHECK_LIMIT();

    if (num_rands) {
      /* Save rator where GC can see it */
      Scheme_Type t;
      arg = (alt_rands 
             ? alt_rands[1+args_already_in_place] 
             : app->args[1+args_already_in_place]);
      t = SCHEME_TYPE(arg);
      if ((num_rands == 1) && ((SAME_TYPE(scheme_local_type, t)
                                && ((SCHEME_GET_LOCAL_FLAGS(arg) != SCHEME_LOCAL_FLONUM)))
			       || (t >= _scheme_values_types_))) {
	/* App of something complex to a local variable. We
	   can move the proc directly to V1. */
	jit_movr_p(JIT_V1, JIT_R0);
	proc_already_in_place = 1;
      } else {
	mz_rs_stxi(num_rands - 1 + offset, JIT_R0);
        if (need_safety)
          need_safety--;
      }
    } else {
      jit_movr_p(JIT_V1, JIT_R0);
    }
  }
  /* not sync'd...*/

  for (i = 0; i < num_rands; i++) {
    PAUSE_JIT_DATA();
    arg = (alt_rands 
           ? alt_rands[i+1+args_already_in_place] 
           : app->args[i+1+args_already_in_place]);
    if (need_safety && !scheme_is_non_gc(arg, INIT_SIMPLE_DEPTH)) {
      scheme_stack_safety(jitter, need_safety - i, offset + i);
      CHECK_LIMIT();
      need_safety = 0;
    }
#ifdef USE_FLONUM_UNBOXING
    if (direct_self 
        && is_tail
        && (SCHEME_CLOSURE_DATA_FLAGS(jitter->self_data) & CLOS_HAS_TYPED_ARGS)
        && (CLOSURE_ARGUMENT_IS_FLONUM(jitter->self_data, i+args_already_in_place))) {
      
      int directly;
      jitter->unbox++;
      if (scheme_can_unbox_inline(arg, 5, JIT_FPR_NUM-1, 0))
        directly = 2;
      else if (scheme_can_unbox_directly(arg))
        directly = 1;
      else
        directly = 0;
      scheme_generate_unboxed(arg, jitter, directly, 1);
      --jitter->unbox;
      --jitter->unbox_depth;
      CHECK_LIMIT();
      scheme_generate_flonum_local_unboxing(jitter, 0);
      CHECK_LIMIT();
      if (SAME_TYPE(SCHEME_TYPE(arg), scheme_local_type)) {
        /* Keep local Scheme_Object view, in case a box has been allocated */
        int apos;
        apos = mz_remap(SCHEME_LOCAL_POS(arg));
        mz_rs_ldxi(JIT_R0, apos);
      } else {
        (void)jit_movi_p(JIT_R0, NULL);
      }
    } else
#endif
      scheme_generate_non_tail(arg, jitter, 0, !need_non_tail, 0); /* sync'd below */
    RESUME_JIT_DATA();
    CHECK_LIMIT();
    if ((i == num_rands - 1) && !direct_prim && !reorder_ok && !direct_self && !proc_already_in_place) {
      /* Move rator back to register: */
      mz_rs_ldxi(JIT_V1, i + offset);
    }
    if ((!direct_prim || (num_rands > 1) || (no_call == 2))
	&& (!direct_self || !is_tail || no_call || (i + 1 < num_rands))) {
      mz_rs_stxi(i + offset, JIT_R0);
    }
  }
  /* not sync'd... */

  if (need_non_tail) {
    /* Uses JIT_R2: */
    scheme_generate_non_tail_mark_pos_suffix(jitter);
    CHECK_LIMIT();
  }

  if (direct_prim) {
    if (!no_call) {
      (void)jit_movi_p(JIT_V1, ((Scheme_Primitive_Proc *)rator)->prim_val);
      if (num_rands == 1) {
        mz_runstack_unskipped(jitter, 1);
      } else {
        mz_rs_sync();
        JIT_UPDATE_THREAD_RSPTR_IF_NEEDED();
      }
      LOG_IT(("direct: %s\n", ((Scheme_Primitive_Proc *)rator)->name));
    }
  }

  if (reorder_ok) {
    if ((no_call < 2) && !apply_to_list) {
      scheme_generate(rator, jitter, 0, 0, 0, JIT_V1, NULL); /* sync'd below, or not */
    }
    CHECK_LIMIT();
  }

  if (!no_call)
    mz_rs_sync();

  END_JIT_DATA(20);

  if (direct_prim || direct_native || direct_self || nontail_self)
    scheme_direct_call_count++;
  else
    scheme_indirect_call_count++;

  if (direct_native && extract_case) {
    /* extract case from case-lambda */
    jit_ldxi_p(JIT_V1, JIT_V1, extract_case);
  }

  if (no_call) {
    /* leave actual call to inlining code */
  } else if (!(direct_self && is_tail)
             && (num_rands >= MAX_SHARED_CALL_RANDS)) {
    LOG_IT(("<-many args\n"));
    if (is_tail) {
      scheme_mz_flostack_restore(jitter, 0, 0, 1, 1);
      if (direct_prim) {
        generate_direct_prim_tail_call(jitter, num_rands);
      } else {
        if (args_already_in_place) {
          jit_movi_l(JIT_R2, args_already_in_place);
          mz_set_local_p(JIT_R2, JIT_LOCAL2);
        }
	scheme_generate_tail_call(jitter, num_rands, direct_native, jitter->need_set_rs, 1);
      }
    } else {
      if (direct_prim)
	generate_direct_prim_non_tail_call(jitter, num_rands, multi_ok, 0);
      else {
        if (nontail_self) {
          generate_nontail_self_setup(jitter);
        }
	scheme_generate_non_tail_call(jitter, num_rands, direct_native, jitter->need_set_rs, 
                                      multi_ok, nontail_self, 0, 1);
      }
    }
  } else {
    /* Jump to code to implement a tail call for num_rands arguments */
    void *code;
    int dp = (direct_prim ? 1 : (direct_native ? (1 + direct_native + (nontail_self ? 1 : 0)) : 0));
    if (is_tail) {
      if (!sjc.shared_tail_code[dp][num_rands]) {
	code = scheme_generate_shared_call(num_rands, jitter, multi_ok, is_tail, direct_prim, direct_native, 0);
	sjc.shared_tail_code[dp][num_rands] = code;
      }
      code = sjc.shared_tail_code[dp][num_rands];
      if (direct_self) {
        LOG_IT(("<-self\n"));
	generate_self_tail_call(rator, jitter, num_rands, code, args_already_in_place, app, alt_rands);
	CHECK_LIMIT();
      } else {
        scheme_mz_flostack_restore(jitter, 0, 0, 1, 1);
        LOG_IT(("<-tail\n"));
        if (args_already_in_place) {
          jit_movi_l(JIT_R2, args_already_in_place);
          mz_set_local_p(JIT_R2, JIT_LOCAL2);
        }
        if (apply_to_list) {
          jit_movi_i(JIT_V1, num_rands);
          (void)jit_jmpi(sjc.apply_to_list_tail_code);
        } else {
          (void)jit_jmpi(code);
        }
      }
    } else {
      int mo = (multi_ok ? 1 : 0);

      if (!sjc.shared_non_tail_code[dp][num_rands][mo]) {
        scheme_ensure_retry_available(jitter, multi_ok);
	code = scheme_generate_shared_call(num_rands, jitter, multi_ok, is_tail, direct_prim, direct_native, nontail_self);
	sjc.shared_non_tail_code[dp][num_rands][mo] = code;
      }
      LOG_IT(("<-non-tail %d %d %d\n", dp, num_rands, mo));
      code = sjc.shared_non_tail_code[dp][num_rands][mo];

      if (nontail_self) {
        generate_nontail_self_setup(jitter);
      }

      if (apply_to_list) {
        jit_movi_i(JIT_V1, num_rands);
        if (multi_ok)
          (void)jit_calli(sjc.apply_to_list_multi_ok_code);
        else
          (void)jit_calli(sjc.apply_to_list_code);
      } else {
        (void)jit_calli(code);
      }

      if (direct_prim) {
        if (num_rands == 1) {
          /* Popped single argument after return of prim: */
          jitter->need_set_rs = 1;
        } else {
          /* Runstack is up-to-date: */
          jitter->need_set_rs = 0;
        }
      } else {
        /* Otherwise, we may have called native code, which may have left
           the runstack register out of sync with scheme_current_runstack. */
        jitter->need_set_rs = 1;
      }
    }
  }

  END_JIT_DATA(need_non_tail ? 22 : 4);
    
  return is_tail ? 2 : 1;
}

#endif