; ModuleID = 'incr_module_12.submodule.0x31b24d34d4e3bac5.ll'
source_filename = "incr_module_12"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::mersenne_twister_engine" = type { [624 x i64], i64 }

$_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE4seedEm = comdat any

@gen = external dso_local global %"class.std::mersenne_twister_engine", align 8
@llvm.global_ctors = external global [1 x { i32, ptr, ptr }]

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl.__cxx_global_var_init.0() #0 section ".text.startup"

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEC2Em(ptr noundef nonnull align 8 dereferenceable(5000), i64 noundef) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
define linkonce_odr dso_local void @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE4seedEm(ptr noundef nonnull align 8 dereferenceable(5000) %this, i64 noundef %__sd) #2 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__sd.addr = alloca i64, align 8
  %__i = alloca i64, align 8
  %__x = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__sd, ptr %__sd.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load i64, ptr %__sd.addr, align 8
  %call = call noundef i64 @_ZNSt8__detail5__modImLm4294967296ELm1ELm0EEET_S1_(i64 noundef %0)
  %_M_x = getelementptr inbounds %"class.std::mersenne_twister_engine", ptr %this1, i32 0, i32 0
  %arrayidx = getelementptr inbounds [624 x i64], ptr %_M_x, i64 0, i64 0
  store i64 %call, ptr %arrayidx, align 8
  store i64 1, ptr %__i, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %1 = load i64, ptr %__i, align 8
  %cmp = icmp ult i64 %1, 624
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %_M_x2 = getelementptr inbounds %"class.std::mersenne_twister_engine", ptr %this1, i32 0, i32 0
  %2 = load i64, ptr %__i, align 8
  %sub = sub i64 %2, 1
  %arrayidx3 = getelementptr inbounds [624 x i64], ptr %_M_x2, i64 0, i64 %sub
  %3 = load i64, ptr %arrayidx3, align 8
  store i64 %3, ptr %__x, align 8
  %4 = load i64, ptr %__x, align 8
  %shr = lshr i64 %4, 30
  %5 = load i64, ptr %__x, align 8
  %xor = xor i64 %5, %shr
  store i64 %xor, ptr %__x, align 8
  %6 = load i64, ptr %__x, align 8
  %mul = mul i64 %6, 1812433253
  store i64 %mul, ptr %__x, align 8
  %7 = load i64, ptr %__i, align 8
  %call4 = call noundef i64 @_ZNSt8__detail5__modImLm624ELm1ELm0EEET_S1_(i64 noundef %7)
  %8 = load i64, ptr %__x, align 8
  %add = add i64 %8, %call4
  store i64 %add, ptr %__x, align 8
  %9 = load i64, ptr %__x, align 8
  %call5 = call noundef i64 @_ZNSt8__detail5__modImLm4294967296ELm1ELm0EEET_S1_(i64 noundef %9)
  %_M_x6 = getelementptr inbounds %"class.std::mersenne_twister_engine", ptr %this1, i32 0, i32 0
  %10 = load i64, ptr %__i, align 8
  %arrayidx7 = getelementptr inbounds [624 x i64], ptr %_M_x6, i64 0, i64 %10
  store i64 %call5, ptr %arrayidx7, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %11 = load i64, ptr %__i, align 8
  %inc = add i64 %11, 1
  store i64 %inc, ptr %__i, align 8
  br label %for.cond, !llvm.loop !6

for.end:                                          ; preds = %for.cond
  %_M_p = getelementptr inbounds %"class.std::mersenne_twister_engine", ptr %this1, i32 0, i32 1
  store i64 624, ptr %_M_p, align 8
  ret void
}

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef i64 @_ZNSt8__detail5__modImLm4294967296ELm1ELm0EEET_S1_(i64 noundef) #2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef i64 @_ZNSt8__detail5__modImLm624ELm1ELm0EEET_S1_(i64 noundef) #2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt8__detail4_ModImLm4294967296ELm1ELm0ELb1ELb1EE6__calcEm(i64 noundef) #3 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt8__detail4_ModImLm624ELm1ELm0ELb1ELb1EE6__calcEm(i64 noundef) #3 align 2

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl._GLOBAL__sub_I_incr_module_12.1() #0 section ".text.startup"

attributes #0 = { noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
