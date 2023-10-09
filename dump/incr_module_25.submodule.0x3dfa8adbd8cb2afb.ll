; ModuleID = 'incr_module_25.submodule.0x3dfa8adbd8cb2afb.ll'
source_filename = "incr_module_25"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.7 = type { double, double, double }

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @_ZngRK4Vec3(ptr noalias sret(%struct.Vec3.7) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %v) #0 {
entry:
  %v.addr = alloca ptr, align 8
  store ptr %v, ptr %v.addr, align 8
  %x = getelementptr inbounds %struct.Vec3.7, ptr %agg.result, i32 0, i32 0
  %0 = load ptr, ptr %v.addr, align 8
  %x1 = getelementptr inbounds %struct.Vec3.7, ptr %0, i32 0, i32 0
  %1 = load double, ptr %x1, align 8
  %fneg = fneg double %1
  store double %fneg, ptr %x, align 8
  %y = getelementptr inbounds %struct.Vec3.7, ptr %agg.result, i32 0, i32 1
  %2 = load ptr, ptr %v.addr, align 8
  %y2 = getelementptr inbounds %struct.Vec3.7, ptr %2, i32 0, i32 1
  %3 = load double, ptr %y2, align 8
  %fneg3 = fneg double %3
  store double %fneg3, ptr %y, align 8
  %z = getelementptr inbounds %struct.Vec3.7, ptr %agg.result, i32 0, i32 2
  %4 = load ptr, ptr %v.addr, align 8
  %z4 = getelementptr inbounds %struct.Vec3.7, ptr %4, i32 0, i32 2
  %5 = load double, ptr %z4, align 8
  %fneg5 = fneg double %5
  store double %fneg5, ptr %z, align 8
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
