; ModuleID = 'incr_module_20.submodule.0x3e802919b0e04509.ll'
source_filename = "incr_module_20"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::normal_distribution.0" = type <{ %"struct.std::normal_distribution<>::param_type.1", double, i8, [7 x i8] }>
%"struct.std::normal_distribution<>::param_type.1" = type { double, double }
%"class.std::mersenne_twister_engine.3" = type { [624 x i64], i64 }
%struct.Vec3 = type { double, double, double }
%"struct.std::__detail::_Adaptor" = type { ptr }

$_ZNSt19normal_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_RKNS0_10param_typeE = comdat any

@nDist = external global %"class.std::normal_distribution.0", align 8
@gen = external global %"class.std::mersenne_twister_engine.3", align 8

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_Z12randomVectorv(ptr noalias sret(%struct.Vec3) align 8) #0

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt19normal_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_(ptr noundef nonnull align 8 dereferenceable(25), ptr noundef nonnull align 8 dereferenceable(5000)) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
define linkonce_odr dso_local noundef double @_ZNSt19normal_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_RKNS0_10param_typeE(ptr noundef nonnull align 8 dereferenceable(25) %this, ptr noundef nonnull align 8 dereferenceable(5000) %__urng, ptr noundef nonnull align 8 dereferenceable(16) %__param) #0 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__urng.addr = alloca ptr, align 8
  %__param.addr = alloca ptr, align 8
  %__ret = alloca double, align 8
  %__aurng = alloca %"struct.std::__detail::_Adaptor", align 8
  %__x = alloca double, align 8
  %__y = alloca double, align 8
  %__r2 = alloca double, align 8
  %__mult = alloca double, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %__urng, ptr %__urng.addr, align 8
  store ptr %__param, ptr %__param.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load ptr, ptr %__urng.addr, align 8
  call void @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEC2ERS2_(ptr noundef nonnull align 8 dereferenceable(8) %__aurng, ptr noundef nonnull align 8 dereferenceable(5000) %0)
  %_M_saved_available = getelementptr inbounds %"class.std::normal_distribution.0", ptr %this1, i32 0, i32 2
  %1 = load i8, ptr %_M_saved_available, align 8
  %tobool = trunc i8 %1 to i1
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %_M_saved_available2 = getelementptr inbounds %"class.std::normal_distribution.0", ptr %this1, i32 0, i32 2
  store i8 0, ptr %_M_saved_available2, align 8
  %_M_saved = getelementptr inbounds %"class.std::normal_distribution.0", ptr %this1, i32 0, i32 1
  %2 = load double, ptr %_M_saved, align 8
  store double %2, ptr %__ret, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  br label %do.body

do.body:                                          ; preds = %lor.end, %if.else
  %call = call noundef double @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEclEv(ptr noundef nonnull align 8 dereferenceable(8) %__aurng)
  %3 = call double @llvm.fmuladd.f64(double 2.000000e+00, double %call, double -1.000000e+00)
  store double %3, ptr %__x, align 8
  %call3 = call noundef double @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEclEv(ptr noundef nonnull align 8 dereferenceable(8) %__aurng)
  %4 = call double @llvm.fmuladd.f64(double 2.000000e+00, double %call3, double -1.000000e+00)
  store double %4, ptr %__y, align 8
  %5 = load double, ptr %__x, align 8
  %6 = load double, ptr %__x, align 8
  %7 = load double, ptr %__y, align 8
  %8 = load double, ptr %__y, align 8
  %mul4 = fmul double %7, %8
  %9 = call double @llvm.fmuladd.f64(double %5, double %6, double %mul4)
  store double %9, ptr %__r2, align 8
  br label %do.cond

do.cond:                                          ; preds = %do.body
  %10 = load double, ptr %__r2, align 8
  %cmp = fcmp ogt double %10, 1.000000e+00
  br i1 %cmp, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %do.cond
  %11 = load double, ptr %__r2, align 8
  %cmp5 = fcmp oeq double %11, 0.000000e+00
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %do.cond
  %12 = phi i1 [ true, %do.cond ], [ %cmp5, %lor.rhs ]
  br i1 %12, label %do.body, label %do.end, !llvm.loop !6

do.end:                                           ; preds = %lor.end
  %13 = load double, ptr %__r2, align 8
  %call6 = call double @log(double noundef %13) #5
  %mul = fmul double -2.000000e+00, %call6
  %14 = load double, ptr %__r2, align 8
  %div = fdiv double %mul, %14
  %call7 = call double @sqrt(double noundef %div) #5
  store double %call7, ptr %__mult, align 8
  %15 = load double, ptr %__x, align 8
  %16 = load double, ptr %__mult, align 8
  %mul8 = fmul double %15, %16
  %_M_saved9 = getelementptr inbounds %"class.std::normal_distribution.0", ptr %this1, i32 0, i32 1
  store double %mul8, ptr %_M_saved9, align 8
  %_M_saved_available10 = getelementptr inbounds %"class.std::normal_distribution.0", ptr %this1, i32 0, i32 2
  store i8 1, ptr %_M_saved_available10, align 8
  %17 = load double, ptr %__y, align 8
  %18 = load double, ptr %__mult, align 8
  %mul11 = fmul double %17, %18
  store double %mul11, ptr %__ret, align 8
  br label %if.end

if.end:                                           ; preds = %do.end, %if.then
  %19 = load double, ptr %__ret, align 8
  %20 = load ptr, ptr %__param.addr, align 8
  %call12 = call noundef double @_ZNKSt19normal_distributionIdE10param_type6stddevEv(ptr noundef nonnull align 8 dereferenceable(16) %20)
  %21 = load ptr, ptr %__param.addr, align 8
  %call14 = call noundef double @_ZNKSt19normal_distributionIdE10param_type4meanEv(ptr noundef nonnull align 8 dereferenceable(16) %21)
  %22 = call double @llvm.fmuladd.f64(double %19, double %call12, double %call14)
  store double %22, ptr %__ret, align 8
  %23 = load double, ptr %__ret, align 8
  ret double %23
}

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
