; ModuleID = 'incr_module_26.submodule.0x699926de8d3c68a3.ll'
source_filename = "incr_module_26"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.8 = type { double, double, double }

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @_ZmlRK4Vec3d(ptr noalias sret(%struct.Vec3.8) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %lhs, double noundef %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca double, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store double %rhs, ptr %rhs.addr, align 8
  %x = getelementptr inbounds %struct.Vec3.8, ptr %agg.result, i32 0, i32 0
  %0 = load ptr, ptr %lhs.addr, align 8
  %x1 = getelementptr inbounds %struct.Vec3.8, ptr %0, i32 0, i32 0
  %1 = load double, ptr %x1, align 8
  %2 = load double, ptr %rhs.addr, align 8
  %mul = fmul double %1, %2
  store double %mul, ptr %x, align 8
  %y = getelementptr inbounds %struct.Vec3.8, ptr %agg.result, i32 0, i32 1
  %3 = load ptr, ptr %lhs.addr, align 8
  %y2 = getelementptr inbounds %struct.Vec3.8, ptr %3, i32 0, i32 1
  %4 = load double, ptr %y2, align 8
  %5 = load double, ptr %rhs.addr, align 8
  %mul3 = fmul double %4, %5
  store double %mul3, ptr %y, align 8
  %z = getelementptr inbounds %struct.Vec3.8, ptr %agg.result, i32 0, i32 2
  %6 = load ptr, ptr %lhs.addr, align 8
  %z4 = getelementptr inbounds %struct.Vec3.8, ptr %6, i32 0, i32 2
  %7 = load double, ptr %z4, align 8
  %8 = load double, ptr %rhs.addr, align 8
  %mul5 = fmul double %7, %8
  store double %mul5, ptr %z, align 8
  ret void
}

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
