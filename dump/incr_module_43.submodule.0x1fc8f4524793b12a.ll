; ModuleID = 'incr_module_43.submodule.0x1fc8f4524793b12a.ll'
source_filename = "incr_module_43"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Color = type { double, double, double }

$_ZN5ColorC2Eddd = comdat any

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_ZplRK5ColorS1_(ptr noalias sret(%struct.Color) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %lhs, ptr noundef nonnull align 8 dereferenceable(24) %rhs) #0 {
entry:
  %lhs.addr = alloca ptr, align 8
  %rhs.addr = alloca ptr, align 8
  store ptr %lhs, ptr %lhs.addr, align 8
  store ptr %rhs, ptr %rhs.addr, align 8
  %0 = load ptr, ptr %lhs.addr, align 8
  %r = getelementptr inbounds %struct.Color, ptr %0, i32 0, i32 0
  %1 = load double, ptr %r, align 8
  %2 = load ptr, ptr %rhs.addr, align 8
  %r1 = getelementptr inbounds %struct.Color, ptr %2, i32 0, i32 0
  %3 = load double, ptr %r1, align 8
  %add = fadd double %1, %3
  %4 = load ptr, ptr %lhs.addr, align 8
  %g = getelementptr inbounds %struct.Color, ptr %4, i32 0, i32 1
  %5 = load double, ptr %g, align 8
  %6 = load ptr, ptr %rhs.addr, align 8
  %g2 = getelementptr inbounds %struct.Color, ptr %6, i32 0, i32 1
  %7 = load double, ptr %g2, align 8
  %add3 = fadd double %5, %7
  %8 = load ptr, ptr %lhs.addr, align 8
  %b = getelementptr inbounds %struct.Color, ptr %8, i32 0, i32 2
  %9 = load double, ptr %b, align 8
  %10 = load ptr, ptr %rhs.addr, align 8
  %b4 = getelementptr inbounds %struct.Color, ptr %10, i32 0, i32 2
  %11 = load double, ptr %b4, align 8
  %add5 = fadd double %9, %11
  call void @_ZN5ColorC2Eddd(ptr noundef nonnull align 8 dereferenceable(24) %agg.result, double noundef %add, double noundef %add3, double noundef %add5)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZN5ColorC2Eddd(ptr noundef nonnull align 8 dereferenceable(24) %this, double noundef %r, double noundef %g, double noundef %b) #1 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %r.addr = alloca double, align 8
  %g.addr = alloca double, align 8
  %b.addr = alloca double, align 8
  store ptr %this, ptr %this.addr, align 8
  store double %r, ptr %r.addr, align 8
  store double %g, ptr %g.addr, align 8
  store double %b, ptr %b.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %r2 = getelementptr inbounds %struct.Color, ptr %this1, i32 0, i32 0
  %0 = load double, ptr %r.addr, align 8
  store double %0, ptr %r2, align 8
  %g3 = getelementptr inbounds %struct.Color, ptr %this1, i32 0, i32 1
  %1 = load double, ptr %g.addr, align 8
  store double %1, ptr %g3, align 8
  %b4 = getelementptr inbounds %struct.Color, ptr %this1, i32 0, i32 2
  %2 = load double, ptr %b.addr, align 8
  store double %2, ptr %b4, align 8
  ret void
}

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
