; ModuleID = 'incr_module_20.submodule.0xda00e3e9f538eedd.ll'
source_filename = "incr_module_20"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::normal_distribution.0" = type <{ %"struct.std::normal_distribution<>::param_type.1", double, i8, [7 x i8] }>
%"struct.std::normal_distribution<>::param_type.1" = type { double, double }
%"class.std::mersenne_twister_engine.3" = type { [624 x i64], i64 }
%struct.Vec3 = type { double, double, double }

$_ZSt18generate_canonicalIdLm53ESt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEET_RT1_ = comdat any

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
define linkonce_odr dso_local noundef double @_ZSt18generate_canonicalIdLm53ESt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEET_RT1_(ptr noundef nonnull align 8 dereferenceable(5000) %__urng) #0 comdat {
entry:
  %__urng.addr = alloca ptr, align 8
  %__b = alloca i64, align 8
  %__r = alloca x86_fp80, align 16
  %__log2r = alloca i64, align 8
  %__m = alloca i64, align 8
  %ref.tmp = alloca i64, align 8
  %ref.tmp6 = alloca i64, align 8
  %__ret = alloca double, align 8
  %__sum = alloca double, align 8
  %__tmp = alloca double, align 8
  %__k = alloca i64, align 8
  store ptr %__urng, ptr %__urng.addr, align 8
  store i64 53, ptr %__b, align 8
  %0 = load ptr, ptr %__urng.addr, align 8
  %call = call noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3maxEv()
  %conv = uitofp i64 %call to x86_fp80
  %1 = load ptr, ptr %__urng.addr, align 8
  %call1 = call noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3minEv()
  %conv2 = uitofp i64 %call1 to x86_fp80
  %sub = fsub x86_fp80 %conv, %conv2
  %add = fadd x86_fp80 %sub, 0xK3FFF8000000000000000
  store x86_fp80 %add, ptr %__r, align 16
  %2 = load x86_fp80, ptr %__r, align 16
  %call3 = call noundef x86_fp80 @_ZSt3loge(x86_fp80 noundef %2)
  %call4 = call noundef x86_fp80 @_ZSt3loge(x86_fp80 noundef 0xK40008000000000000000)
  %div = fdiv x86_fp80 %call3, %call4
  %conv5 = fptoui x86_fp80 %div to i64
  store i64 %conv5, ptr %__log2r, align 8
  store i64 1, ptr %ref.tmp, align 8
  %3 = load i64, ptr %__log2r, align 8
  %add7 = add i64 53, %3
  %sub8 = sub i64 %add7, 1
  %4 = load i64, ptr %__log2r, align 8
  %div9 = udiv i64 %sub8, %4
  store i64 %div9, ptr %ref.tmp6, align 8
  %call10 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3maxImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp, ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp6)
  %5 = load i64, ptr %call10, align 8
  store i64 %5, ptr %__m, align 8
  store double 0.000000e+00, ptr %__sum, align 8
  store double 1.000000e+00, ptr %__tmp, align 8
  %6 = load i64, ptr %__m, align 8
  store i64 %6, ptr %__k, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %7 = load i64, ptr %__k, align 8
  %cmp = icmp ne i64 %7, 0
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %8 = load ptr, ptr %__urng.addr, align 8
  %call11 = call noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv(ptr noundef nonnull align 8 dereferenceable(5000) %8)
  %9 = load ptr, ptr %__urng.addr, align 8
  %call12 = call noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3minEv()
  %sub13 = sub i64 %call11, %call12
  %conv14 = uitofp i64 %sub13 to double
  %10 = load double, ptr %__tmp, align 8
  %11 = load double, ptr %__sum, align 8
  %12 = call double @llvm.fmuladd.f64(double %conv14, double %10, double %11)
  store double %12, ptr %__sum, align 8
  %13 = load x86_fp80, ptr %__r, align 16
  %14 = load double, ptr %__tmp, align 8
  %conv15 = fpext double %14 to x86_fp80
  %mul = fmul x86_fp80 %conv15, %13
  %conv16 = fptrunc x86_fp80 %mul to double
  store double %conv16, ptr %__tmp, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %15 = load i64, ptr %__k, align 8
  %dec = add i64 %15, -1
  store i64 %dec, ptr %__k, align 8
  br label %for.cond, !llvm.loop !6

for.end:                                          ; preds = %for.cond
  %16 = load double, ptr %__sum, align 8
  %17 = load double, ptr %__tmp, align 8
  %div17 = fdiv double %16, %17
  store double %div17, ptr %__ret, align 8
  %18 = load double, ptr %__ret, align 8
  %cmp18 = fcmp oge double %18, 1.000000e+00
  br i1 %cmp18, label %if.then, label %if.end

if.then:                                          ; preds = %for.end
  %call20 = call double @nextafter(double noundef 1.000000e+00, double noundef 0.000000e+00) #5
  store double %call20, ptr %__ret, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %for.end
  %19 = load double, ptr %__ret, align 8
  ret double %19
}

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
declare dso_local void @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv(ptr noundef nonnull align 8 dereferenceable(5000)) #4 align 2

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind }

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
