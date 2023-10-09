; ModuleID = 'incr_module_31.submodule.0x59740422b51a1b33.ll'
source_filename = "incr_module_31"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.12 = type { double, double, double }

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24) %lhs, ptr noundef nonnull align 8 dereferenceable(24) %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca ptr, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store ptr %rhs, ptr %rhs.addr, align 8
  %0 = load ptr, ptr %lhs.addr, align 8
  %x = getelementptr inbounds %struct.Vec3.12, ptr %0, i32 0, i32 0
  %1 = load double, ptr %x, align 8
  %2 = load ptr, ptr %rhs.addr, align 8
  %x1 = getelementptr inbounds %struct.Vec3.12, ptr %2, i32 0, i32 0
  %3 = load double, ptr %x1, align 8
  %4 = load ptr, ptr %lhs.addr, align 8
  %y = getelementptr inbounds %struct.Vec3.12, ptr %4, i32 0, i32 1
  %5 = load double, ptr %y, align 8
  %6 = load ptr, ptr %rhs.addr, align 8
  %y2 = getelementptr inbounds %struct.Vec3.12, ptr %6, i32 0, i32 1
  %7 = load double, ptr %y2, align 8
  %mul3 = fmul double %5, %7
  %8 = call double @llvm.fmuladd.f64(double %1, double %3, double %mul3)
  %9 = load ptr, ptr %lhs.addr, align 8
  %z = getelementptr inbounds %struct.Vec3.12, ptr %9, i32 0, i32 2
  %10 = load double, ptr %z, align 8
  %11 = load ptr, ptr %rhs.addr, align 8
  %z4 = getelementptr inbounds %struct.Vec3.12, ptr %11, i32 0, i32 2
  %12 = load double, ptr %z4, align 8
  %13 = call double @llvm.fmuladd.f64(double %10, double %12, double %8)
  ret double %13
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #1

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
