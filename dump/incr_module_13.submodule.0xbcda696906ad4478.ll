; ModuleID = 'incr_module_13.submodule.0xbcda696906ad4478.ll'
source_filename = "incr_module_13"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::normal_distribution" = type <{ %"struct.std::normal_distribution<>::param_type", double, i8, [7 x i8] }>
%"struct.std::normal_distribution<>::param_type" = type { double, double }

$_ZNSt19normal_distributionIdEC2Edd = comdat any

@nDist = external dso_local global %"class.std::normal_distribution", align 8
@llvm.global_ctors = external global [1 x { i32, ptr, ptr }]

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl.__cxx_global_var_init.2() #0 section ".text.startup"

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local void @_ZNSt19normal_distributionIdEC2Edd(ptr noundef nonnull align 8 dereferenceable(25) %this, double noundef %__mean, double noundef %__stddev) #1 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__mean.addr = alloca double, align 8
  %__stddev.addr = alloca double, align 8
  store ptr %this, ptr %this.addr, align 8
  store double %__mean, ptr %__mean.addr, align 8
  store double %__stddev, ptr %__stddev.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_param = getelementptr inbounds %"class.std::normal_distribution", ptr %this1, i32 0, i32 0
  %0 = load double, ptr %__mean.addr, align 8
  %1 = load double, ptr %__stddev.addr, align 8
  call void @_ZNSt19normal_distributionIdE10param_typeC2Edd(ptr noundef nonnull align 8 dereferenceable(16) %_M_param, double noundef %0, double noundef %1)
  %_M_saved = getelementptr inbounds %"class.std::normal_distribution", ptr %this1, i32 0, i32 1
  store double 0.000000e+00, ptr %_M_saved, align 8
  %_M_saved_available = getelementptr inbounds %"class.std::normal_distribution", ptr %this1, i32 0, i32 2
  store i8 0, ptr %_M_saved_available, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt19normal_distributionIdE10param_typeC2Edd(ptr noundef nonnull align 8 dereferenceable(16), double noundef, double noundef) #2 align 2

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl._GLOBAL__sub_I_incr_module_13.3() #0 section ".text.startup"

attributes #0 = { noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
