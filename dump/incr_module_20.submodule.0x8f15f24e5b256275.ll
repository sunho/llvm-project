; ModuleID = 'incr_module_20.submodule.0x8f15f24e5b256275.ll'
source_filename = "incr_module_20"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::normal_distribution.0" = type <{ %"struct.std::normal_distribution<>::param_type.1", double, i8, [7 x i8] }>
%"struct.std::normal_distribution<>::param_type.1" = type { double, double }
%"class.std::mersenne_twister_engine.3" = type { [624 x i64], i64 }
%struct.Vec3 = type { double, double, double }

$_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv = comdat any

@nDist = external global %"class.std::normal_distribution.0", align 8
@gen = external global %"class.std::mersenne_twister_engine.3", align 8

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_Z12randomVectorv(ptr noalias sret(%struct.Vec3) align 8) #0

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt19normal_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_(ptr noundef nonnull align 8 dereferenceable(25), ptr noundef nonnull align 8 dereferenceable(5000)) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt19normal_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_RKNS0_10param_typeE(ptr noundef nonnull align 8 dereferenceable(25), ptr noundef nonnull align 8 dereferenceable(5000), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEC2ERS2_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(5000)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEclEv(ptr noundef nonnull align 8 dereferenceable(8)) #0 align 2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #2

; Function Attrs: nounwind
declare double @sqrt(double noundef) #3

; Function Attrs: nounwind
declare double @log(double noundef) #3

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNKSt19normal_distributionIdE10param_type6stddevEv(ptr noundef nonnull align 8 dereferenceable(16)) #4 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNKSt19normal_distributionIdE10param_type4meanEv(ptr noundef nonnull align 8 dereferenceable(16)) #4 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZSt18generate_canonicalIdLm53ESt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEET_RT1_(ptr noundef nonnull align 8 dereferenceable(5000)) #0

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3maxEv() #4 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3minEv() #4 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef x86_fp80 @_ZSt3loge(x86_fp80 noundef) #4

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3maxImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #4

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv(ptr noundef nonnull align 8 dereferenceable(5000)) #0 align 2

; Function Attrs: nounwind
declare double @nextafter(double noundef, double noundef) #3

; Function Attrs: nounwind
declare x86_fp80 @logl(x86_fp80 noundef) #3

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv(ptr noundef nonnull align 8 dereferenceable(5000) %this) #4 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__upper_mask = alloca i64, align 8
  %__lower_mask = alloca i64, align 8
  %__k = alloca i64, align 8
  %__y = alloca i64, align 8
  %__k12 = alloca i64, align 8
  %__y16 = alloca i64, align 8
  %__y39 = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  store i64 -2147483648, ptr %__upper_mask, align 8
  store i64 2147483647, ptr %__lower_mask, align 8
  store i64 0, ptr %__k, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i64, ptr %__k, align 8
  %cmp = icmp ult i64 %0, 227
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %_M_x = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %1 = load i64, ptr %__k, align 8
  %arrayidx = getelementptr inbounds [624 x i64], ptr %_M_x, i64 0, i64 %1
  %2 = load i64, ptr %arrayidx, align 8
  %and = and i64 %2, -2147483648
  %_M_x2 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %3 = load i64, ptr %__k, align 8
  %add = add i64 %3, 1
  %arrayidx3 = getelementptr inbounds [624 x i64], ptr %_M_x2, i64 0, i64 %add
  %4 = load i64, ptr %arrayidx3, align 8
  %and4 = and i64 %4, 2147483647
  %or = or i64 %and, %and4
  store i64 %or, ptr %__y, align 8
  %_M_x5 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %5 = load i64, ptr %__k, align 8
  %add6 = add i64 %5, 397
  %arrayidx7 = getelementptr inbounds [624 x i64], ptr %_M_x5, i64 0, i64 %add6
  %6 = load i64, ptr %arrayidx7, align 8
  %7 = load i64, ptr %__y, align 8
  %shr = lshr i64 %7, 1
  %xor = xor i64 %6, %shr
  %8 = load i64, ptr %__y, align 8
  %and8 = and i64 %8, 1
  %tobool = icmp ne i64 %and8, 0
  %9 = zext i1 %tobool to i64
  %cond = select i1 %tobool, i64 2567483615, i64 0
  %xor9 = xor i64 %xor, %cond
  %_M_x10 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %10 = load i64, ptr %__k, align 8
  %arrayidx11 = getelementptr inbounds [624 x i64], ptr %_M_x10, i64 0, i64 %10
  store i64 %xor9, ptr %arrayidx11, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %11 = load i64, ptr %__k, align 8
  %inc = add i64 %11, 1
  store i64 %inc, ptr %__k, align 8
  br label %for.cond, !llvm.loop !6

for.end:                                          ; preds = %for.cond
  store i64 227, ptr %__k12, align 8
  br label %for.cond13

for.cond13:                                       ; preds = %for.inc36, %for.end
  %12 = load i64, ptr %__k12, align 8
  %cmp14 = icmp ult i64 %12, 623
  br i1 %cmp14, label %for.body15, label %for.end38

for.body15:                                       ; preds = %for.cond13
  %_M_x17 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %13 = load i64, ptr %__k12, align 8
  %arrayidx18 = getelementptr inbounds [624 x i64], ptr %_M_x17, i64 0, i64 %13
  %14 = load i64, ptr %arrayidx18, align 8
  %and19 = and i64 %14, -2147483648
  %_M_x20 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %15 = load i64, ptr %__k12, align 8
  %add21 = add i64 %15, 1
  %arrayidx22 = getelementptr inbounds [624 x i64], ptr %_M_x20, i64 0, i64 %add21
  %16 = load i64, ptr %arrayidx22, align 8
  %and23 = and i64 %16, 2147483647
  %or24 = or i64 %and19, %and23
  store i64 %or24, ptr %__y16, align 8
  %_M_x25 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %17 = load i64, ptr %__k12, align 8
  %add26 = add i64 %17, -227
  %arrayidx27 = getelementptr inbounds [624 x i64], ptr %_M_x25, i64 0, i64 %add26
  %18 = load i64, ptr %arrayidx27, align 8
  %19 = load i64, ptr %__y16, align 8
  %shr28 = lshr i64 %19, 1
  %xor29 = xor i64 %18, %shr28
  %20 = load i64, ptr %__y16, align 8
  %and30 = and i64 %20, 1
  %tobool31 = icmp ne i64 %and30, 0
  %21 = zext i1 %tobool31 to i64
  %cond32 = select i1 %tobool31, i64 2567483615, i64 0
  %xor33 = xor i64 %xor29, %cond32
  %_M_x34 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %22 = load i64, ptr %__k12, align 8
  %arrayidx35 = getelementptr inbounds [624 x i64], ptr %_M_x34, i64 0, i64 %22
  store i64 %xor33, ptr %arrayidx35, align 8
  br label %for.inc36

for.inc36:                                        ; preds = %for.body15
  %23 = load i64, ptr %__k12, align 8
  %inc37 = add i64 %23, 1
  store i64 %inc37, ptr %__k12, align 8
  br label %for.cond13, !llvm.loop !8

for.end38:                                        ; preds = %for.cond13
  %_M_x40 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %arrayidx41 = getelementptr inbounds [624 x i64], ptr %_M_x40, i64 0, i64 623
  %24 = load i64, ptr %arrayidx41, align 8
  %and42 = and i64 %24, -2147483648
  %_M_x43 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %arrayidx44 = getelementptr inbounds [624 x i64], ptr %_M_x43, i64 0, i64 0
  %25 = load i64, ptr %arrayidx44, align 8
  %and45 = and i64 %25, 2147483647
  %or46 = or i64 %and42, %and45
  store i64 %or46, ptr %__y39, align 8
  %_M_x47 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %arrayidx48 = getelementptr inbounds [624 x i64], ptr %_M_x47, i64 0, i64 396
  %26 = load i64, ptr %arrayidx48, align 8
  %27 = load i64, ptr %__y39, align 8
  %shr49 = lshr i64 %27, 1
  %xor50 = xor i64 %26, %shr49
  %28 = load i64, ptr %__y39, align 8
  %and51 = and i64 %28, 1
  %tobool52 = icmp ne i64 %and51, 0
  %29 = zext i1 %tobool52 to i64
  %cond53 = select i1 %tobool52, i64 2567483615, i64 0
  %xor54 = xor i64 %xor50, %cond53
  %_M_x55 = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 0
  %arrayidx56 = getelementptr inbounds [624 x i64], ptr %_M_x55, i64 0, i64 623
  store i64 %xor54, ptr %arrayidx56, align 8
  %_M_p = getelementptr inbounds %"class.std::mersenne_twister_engine.3", ptr %this1, i32 0, i32 1
  store i64 0, ptr %_M_p, align 8
  ret void
}

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
!8 = distinct !{!8, !7}
