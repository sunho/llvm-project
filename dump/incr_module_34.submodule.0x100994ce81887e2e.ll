; ModuleID = 'incr_module_34.submodule.0x100994ce81887e2e.ll'
source_filename = "incr_module_34"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.15 = type { double, double, double }

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_Z7refractRK4Vec3S1_d(ptr noalias sret(%struct.Vec3.15) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %incoming, ptr noundef nonnull align 8 dereferenceable(24) %normal, double noundef %eta) #0 {
entry:
  %incoming.addr = alloca ptr, align 8
  %normal.addr = alloca ptr, align 8
  %eta.addr = alloca double, align 8
  %cosI = alloca double, align 8
  %sinT2 = alloca double, align 8
  %cosT = alloca double, align 8
  %ref.tmp = alloca %struct.Vec3.15, align 8
  %ref.tmp4 = alloca %struct.Vec3.15, align 8
  store ptr %incoming, ptr %incoming.addr, align 8
  store ptr %normal, ptr %normal.addr, align 8
  store double %eta, ptr %eta.addr, align 8
  %0 = load ptr, ptr %incoming.addr, align 8
  %1 = load ptr, ptr %normal.addr, align 8
  %call = call noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %1)
  %fneg = fneg double %call
  store double %fneg, ptr %cosI, align 8
  %2 = load double, ptr %eta.addr, align 8
  %3 = load double, ptr %eta.addr, align 8
  %mul = fmul double %2, %3
  %4 = load double, ptr %cosI, align 8
  %5 = load double, ptr %cosI, align 8
  %neg = fneg double %4
  %6 = call double @llvm.fmuladd.f64(double %neg, double %5, double 1.000000e+00)
  %mul2 = fmul double %mul, %6
  store double %mul2, ptr %sinT2, align 8
  %7 = load double, ptr %sinT2, align 8
  %cmp = fcmp ogt double %7, 1.000000e+00
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @llvm.memset.p0.i64(ptr align 8 %agg.result, i8 0, i64 24, i1 false)
  br label %return

if.end:                                           ; preds = %entry
  %8 = load double, ptr %sinT2, align 8
  %sub = fsub double 1.000000e+00, %8
  %call3 = call double @sqrt(double noundef %sub) #5
  store double %call3, ptr %cosT, align 8
  %9 = load ptr, ptr %incoming.addr, align 8
  %10 = load double, ptr %eta.addr, align 8
  call void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.15) align 8 %ref.tmp, ptr noundef nonnull align 8 dereferenceable(24) %9, double noundef %10)
  %11 = load ptr, ptr %normal.addr, align 8
  %12 = load double, ptr %eta.addr, align 8
  %13 = load double, ptr %cosI, align 8
  %14 = load double, ptr %cosT, align 8
  %neg6 = fneg double %14
  %15 = call double @llvm.fmuladd.f64(double %12, double %13, double %neg6)
  call void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.15) align 8 %ref.tmp4, ptr noundef nonnull align 8 dereferenceable(24) %11, double noundef %15)
  call void @_ZplRK4Vec3S1_(ptr sret(%struct.Vec3.15) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %ref.tmp, ptr noundef nonnull align 8 dereferenceable(24) %ref.tmp4)
  br label %return

return:                                           ; preds = %if.end, %if.then
  ret void
}

declare noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #1

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #2

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #3

; Function Attrs: nounwind
declare double @sqrt(double noundef) #4

declare void @_ZplRK4Vec3S1_(ptr sret(%struct.Vec3.15) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #1

declare void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.15) align 8, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #1

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #4 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
