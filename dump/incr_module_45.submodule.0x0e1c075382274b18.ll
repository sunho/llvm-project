; ModuleID = 'incr_module_45.submodule.0x0e1c075382274b18.ll'
source_filename = "incr_module_45"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Color.19 = type { double, double, double }

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_ZmlRK5ColorS1_(ptr noalias sret(%struct.Color.19) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %lhs, ptr noundef nonnull align 8 dereferenceable(24) %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca ptr, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store ptr %rhs, ptr %rhs.addr, align 8
  %0 = load ptr, ptr %lhs.addr, align 8
  %r = getelementptr inbounds %struct.Color.19, ptr %0, i32 0, i32 0
  %1 = load double, ptr %r, align 8
  %2 = load ptr, ptr %rhs.addr, align 8
  %r1 = getelementptr inbounds %struct.Color.19, ptr %2, i32 0, i32 0
  %3 = load double, ptr %r1, align 8
  %mul = fmul double %1, %3
  %4 = load ptr, ptr %lhs.addr, align 8
  %g = getelementptr inbounds %struct.Color.19, ptr %4, i32 0, i32 1
  %5 = load double, ptr %g, align 8
  %6 = load ptr, ptr %rhs.addr, align 8
  %g2 = getelementptr inbounds %struct.Color.19, ptr %6, i32 0, i32 1
  %7 = load double, ptr %g2, align 8
  %mul3 = fmul double %5, %7
  %8 = load ptr, ptr %lhs.addr, align 8
  %b = getelementptr inbounds %struct.Color.19, ptr %8, i32 0, i32 2
  %9 = load double, ptr %b, align 8
  %10 = load ptr, ptr %rhs.addr, align 8
  %b4 = getelementptr inbounds %struct.Color.19, ptr %10, i32 0, i32 2
  %11 = load double, ptr %b4, align 8
  %mul5 = fmul double %9, %11
  call void @_ZN5ColorC2Eddd(ptr noundef nonnull align 8 dereferenceable(24) %agg.result, double noundef %mul, double noundef %mul3, double noundef %mul5)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5ColorC2Eddd(ptr noundef nonnull align 8 dereferenceable(24), double noundef, double noundef, double noundef) #1 align 2

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
