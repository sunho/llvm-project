; ModuleID = 'incr_module_24.submodule.0xbf6fce4fc2433b80.ll'
source_filename = "incr_module_24"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.6 = type { double, double, double }

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @_ZmiRK4Vec3S1_(ptr noalias sret(%struct.Vec3.6) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %lhs, ptr noundef nonnull align 8 dereferenceable(24) %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca ptr, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store ptr %rhs, ptr %rhs.addr, align 8
  %x = getelementptr inbounds %struct.Vec3.6, ptr %agg.result, i32 0, i32 0
  %0 = load ptr, ptr %lhs.addr, align 8
  %x1 = getelementptr inbounds %struct.Vec3.6, ptr %0, i32 0, i32 0
  %1 = load double, ptr %x1, align 8
  %2 = load ptr, ptr %rhs.addr, align 8
  %x2 = getelementptr inbounds %struct.Vec3.6, ptr %2, i32 0, i32 0
  %3 = load double, ptr %x2, align 8
  %sub = fsub double %1, %3
  store double %sub, ptr %x, align 8
  %y = getelementptr inbounds %struct.Vec3.6, ptr %agg.result, i32 0, i32 1
  %4 = load ptr, ptr %lhs.addr, align 8
  %y3 = getelementptr inbounds %struct.Vec3.6, ptr %4, i32 0, i32 1
  %5 = load double, ptr %y3, align 8
  %6 = load ptr, ptr %rhs.addr, align 8
  %y4 = getelementptr inbounds %struct.Vec3.6, ptr %6, i32 0, i32 1
  %7 = load double, ptr %y4, align 8
  %sub5 = fsub double %5, %7
  store double %sub5, ptr %y, align 8
  %z = getelementptr inbounds %struct.Vec3.6, ptr %agg.result, i32 0, i32 2
  %8 = load ptr, ptr %lhs.addr, align 8
  %z6 = getelementptr inbounds %struct.Vec3.6, ptr %8, i32 0, i32 2
  %9 = load double, ptr %z6, align 8
  %10 = load ptr, ptr %rhs.addr, align 8
  %z7 = getelementptr inbounds %struct.Vec3.6, ptr %10, i32 0, i32 2
  %11 = load double, ptr %z7, align 8
  %sub8 = fsub double %9, %11
  store double %sub8, ptr %z, align 8
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
