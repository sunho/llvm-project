; ModuleID = 'incr_module_14.submodule.0x7f8b6e23de1af012.ll'
source_filename = "incr_module_14"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::uniform_real_distribution" = type { %"struct.std::uniform_real_distribution<>::param_type" }
%"struct.std::uniform_real_distribution<>::param_type" = type { double, double }

$_ZNSt25uniform_real_distributionIdE10param_typeC2Edd = comdat any

@uDist = external dso_local global %"class.std::uniform_real_distribution", align 8
@llvm.global_ctors = external global [1 x { i32, ptr, ptr }]

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl.__cxx_global_var_init.4() #0 section ".text.startup"

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt25uniform_real_distributionIdEC2Edd(ptr noundef nonnull align 8 dereferenceable(16), double noundef, double noundef) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZNSt25uniform_real_distributionIdE10param_typeC2Edd(ptr noundef nonnull align 8 dereferenceable(16) %this, double noundef %__a, double noundef %__b) #2 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__a.addr = alloca double, align 8
  %__b.addr = alloca double, align 8
  store ptr %this, ptr %this.addr, align 8
  store double %__a, ptr %__a.addr, align 8
  store double %__b, ptr %__b.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_a = getelementptr inbounds %"struct.std::uniform_real_distribution<>::param_type", ptr %this1, i32 0, i32 0
  %0 = load double, ptr %__a.addr, align 8
  store double %0, ptr %_M_a, align 8
  %_M_b = getelementptr inbounds %"struct.std::uniform_real_distribution<>::param_type", ptr %this1, i32 0, i32 1
  %1 = load double, ptr %__b.addr, align 8
  store double %1, ptr %_M_b, align 8
  br label %do.body

do.body:                                          ; preds = %entry
  br label %do.end

do.end:                                           ; preds = %do.body
  ret void
}

; Function Attrs: noinline uwtable
declare hidden void @__orc_lcl._GLOBAL__sub_I_incr_module_14.5() #0 section ".text.startup"

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
