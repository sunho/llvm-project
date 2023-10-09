; ModuleID = 'incr_module_32.submodule.0x6e2bd5d831e38f2d.ll'
source_filename = "incr_module_32"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.13 = type { double, double, double }

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @_Z5crossRK4Vec3S1_(ptr noalias sret(%struct.Vec3.13) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %lhs, ptr noundef nonnull align 8 dereferenceable(24) %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca ptr, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store ptr %rhs, ptr %rhs.addr, align 8
  %x = getelementptr inbounds %struct.Vec3.13, ptr %agg.result, i32 0, i32 0
  %0 = load ptr, ptr %lhs.addr, align 8
  %y = getelementptr inbounds %struct.Vec3.13, ptr %0, i32 0, i32 1
  %1 = load double, ptr %y, align 8
  %2 = load ptr, ptr %rhs.addr, align 8
  %z = getelementptr inbounds %struct.Vec3.13, ptr %2, i32 0, i32 2
  %3 = load double, ptr %z, align 8
  %4 = load ptr, ptr %lhs.addr, align 8
  %z1 = getelementptr inbounds %struct.Vec3.13, ptr %4, i32 0, i32 2
  %5 = load double, ptr %z1, align 8
  %6 = load ptr, ptr %rhs.addr, align 8
  %y2 = getelementptr inbounds %struct.Vec3.13, ptr %6, i32 0, i32 1
  %7 = load double, ptr %y2, align 8
  %mul3 = fmul double %5, %7
  %neg = fneg double %mul3
  %8 = call double @llvm.fmuladd.f64(double %1, double %3, double %neg)
  store double %8, ptr %x, align 8
  %y4 = getelementptr inbounds %struct.Vec3.13, ptr %agg.result, i32 0, i32 1
  %9 = load ptr, ptr %lhs.addr, align 8
  %z5 = getelementptr inbounds %struct.Vec3.13, ptr %9, i32 0, i32 2
  %10 = load double, ptr %z5, align 8
  %11 = load ptr, ptr %rhs.addr, align 8
  %x6 = getelementptr inbounds %struct.Vec3.13, ptr %11, i32 0, i32 0
  %12 = load double, ptr %x6, align 8
  %13 = load ptr, ptr %lhs.addr, align 8
  %x7 = getelementptr inbounds %struct.Vec3.13, ptr %13, i32 0, i32 0
  %14 = load double, ptr %x7, align 8
  %15 = load ptr, ptr %rhs.addr, align 8
  %z8 = getelementptr inbounds %struct.Vec3.13, ptr %15, i32 0, i32 2
  %16 = load double, ptr %z8, align 8
  %mul9 = fmul double %14, %16
  %neg10 = fneg double %mul9
  %17 = call double @llvm.fmuladd.f64(double %10, double %12, double %neg10)
  store double %17, ptr %y4, align 8
  %z11 = getelementptr inbounds %struct.Vec3.13, ptr %agg.result, i32 0, i32 2
  %18 = load ptr, ptr %lhs.addr, align 8
  %x12 = getelementptr inbounds %struct.Vec3.13, ptr %18, i32 0, i32 0
  %19 = load double, ptr %x12, align 8
  %20 = load ptr, ptr %rhs.addr, align 8
  %y13 = getelementptr inbounds %struct.Vec3.13, ptr %20, i32 0, i32 1
  %21 = load double, ptr %y13, align 8
  %22 = load ptr, ptr %lhs.addr, align 8
  %y14 = getelementptr inbounds %struct.Vec3.13, ptr %22, i32 0, i32 1
  %23 = load double, ptr %y14, align 8
  %24 = load ptr, ptr %rhs.addr, align 8
  %x15 = getelementptr inbounds %struct.Vec3.13, ptr %24, i32 0, i32 0
  %25 = load double, ptr %x15, align 8
  %mul16 = fmul double %23, %25
  %neg17 = fneg double %mul16
  %26 = call double @llvm.fmuladd.f64(double %19, double %21, double %neg17)
  store double %26, ptr %z11, align 8
  ret void
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
