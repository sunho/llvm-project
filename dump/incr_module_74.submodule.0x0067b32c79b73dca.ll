; ModuleID = 'incr_module_74.submodule.0x0067b32c79b73dca.ll'
source_filename = "incr_module_74"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::uniform_real_distribution.25" = type { %"struct.std::uniform_real_distribution<>::param_type.26" }
%"struct.std::uniform_real_distribution<>::param_type.26" = type { double, double }
%"class.std::mersenne_twister_engine.27" = type { [624 x i64], i64 }
%"struct.std::nullopt_t" = type { i8 }
%"struct.std::in_place_t" = type { i8 }
%"class.std::basic_ostream.29" = type { ptr, %"class.std::basic_ios.30" }
%"class.std::basic_ios.30" = type { %"class.std::ios_base.31", ptr, i8, i8, ptr, ptr, ptr, ptr }
%"class.std::ios_base.31" = type { ptr, i64, i64, i32, i32, i32, ptr, %"struct.std::ios_base::_Words.32", [8 x %"struct.std::ios_base::_Words.32"], i32, ptr, %"class.std::locale.33" }
%"struct.std::ios_base::_Words.32" = type { ptr, i64 }
%"class.std::locale.33" = type { ptr }
%struct.Color.23 = type { double, double, double }
%struct.Ray = type { %struct.Vec3.24, %struct.Vec3.24 }
%struct.Vec3.24 = type { double, double, double }
%"class.std::optional.40" = type { %"struct.std::_Optional_base.41" }
%"struct.std::_Optional_base.41" = type { %"struct.std::_Optional_payload.43" }
%"struct.std::_Optional_payload.43" = type { %"struct.std::_Optional_payload_base.base.45", [7 x i8] }
%"struct.std::_Optional_payload_base.base.45" = type <{ %"union.std::_Optional_payload_base<Intersection>::_Storage", i8 }>
%"union.std::_Optional_payload_base<Intersection>::_Storage" = type { %struct.Intersection }
%struct.Intersection = type { double, %struct.Vec3.24, %struct.Vec3.24, ptr }
%"struct.std::_Optional_payload_base.44" = type <{ %"union.std::_Optional_payload_base<Intersection>::_Storage", i8, [7 x i8] }>
%"struct.__gnu_cxx::__aligned_membuf" = type { [8 x i8] }
%"struct.std::_Optional_payload_base" = type <{ %"union.std::_Optional_payload_base<double>::_Storage", i8, [7 x i8] }>
%"union.std::_Optional_payload_base<double>::_Storage" = type { double }

$_ZNSt22_Optional_payload_baseI12IntersectionEC2Ev = comdat any

$_ZNK9__gnu_cxx16__aligned_membufIP5ShapeE7_M_addrEv = comdat any

$_ZNSt22_Optional_payload_baseIdE6_M_getEv = comdat any

$_ZNSt17_Optional_payloadI12IntersectionLb1ELb1ELb1EECI2St22_Optional_payload_baseIS0_EIJS0_EEESt10in_place_tDpOT_ = comdat any

$_ZNSt15__new_allocatorISt10_List_nodeI5ColorEE8allocateEmPKv = comdat any

$_ZNSt15__new_allocatorISt10_List_nodeI5ColorEE10deallocateEPS2_m = comdat any

@__orc_lcl..str.8 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.1.9 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.2.10 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.3.11 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.4.12 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.5.13 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.6.14 = external hidden constant [8 x i8], align 1
@__orc_lcl..str.7.15 = external hidden constant [11 x i8], align 1
@__orc_lcl..str.8.16 = external hidden constant [3 x i8], align 1
@__orc_lcl..str.9.17 = external hidden constant [11 x i8], align 1
@_ZTV10Lambertian = external dso_local constant { [3 x ptr] }, align 8
@_ZTVN10__cxxabiv120__si_class_type_infoE = external global [0 x ptr]
@_ZTS10Lambertian = external dso_local constant [13 x i8], align 1
@_ZTVN10__cxxabiv117__class_type_infoE = external global [0 x ptr]
@_ZTS8Material = external dso_local constant [10 x i8], align 1
@_ZTI8Material = external dso_local constant { ptr, ptr }, align 8
@_ZTI10Lambertian = external dso_local constant { ptr, ptr, ptr }, align 8
@_ZTV8Material = external dso_local constant { [3 x ptr] }, align 8
@_ZTV10Dielectric = external dso_local constant { [3 x ptr] }, align 8
@_ZTS10Dielectric = external dso_local constant [13 x i8], align 1
@_ZTI10Dielectric = external dso_local constant { ptr, ptr, ptr }, align 8
@_ZTVN10__cxxabiv119__pointer_type_infoE = external global [0 x ptr]
@_ZTSPDoFeeE = external dso_local constant [8 x i8], align 1
@_ZTVN10__cxxabiv120__function_type_infoE = external global [0 x ptr]
@_ZTSFeeE = external dso_local constant [5 x i8], align 1
@_ZTIFeeE = external dso_local constant { ptr, ptr }, align 8
@_ZTIPDoFeeE = external dso_local constant { ptr, ptr, i32, ptr }, align 8
@uDist = external global %"class.std::uniform_real_distribution.25", align 8
@gen = external global %"class.std::mersenne_twister_engine.27", align 8
@_ZTV8Metallic = external dso_local constant { [3 x ptr] }, align 8
@_ZTS8Metallic = external dso_local constant [10 x i8], align 1
@_ZTI8Metallic = external dso_local constant { ptr, ptr, ptr }, align 8
@_ZTV5Plane = external dso_local constant { [4 x ptr] }, align 8
@_ZTS5Plane = external dso_local constant [7 x i8], align 1
@_ZTS5Shape = external dso_local constant [7 x i8], align 1
@_ZTI5Shape = external dso_local constant { ptr, ptr }, align 8
@_ZTI5Plane = external dso_local constant { ptr, ptr, ptr }, align 8
@_ZTV5Shape = external dso_local constant { [4 x ptr] }, align 8
@_ZSt7nullopt = external dso_local constant %"struct.std::nullopt_t", align 1
@_ZSt8in_place = external dso_local constant %"struct.std::in_place_t", align 1
@_ZTV6Sphere = external dso_local constant { [4 x ptr] }, align 8
@_ZTS6Sphere = external dso_local constant [8 x i8], align 1
@_ZTI6Sphere = external dso_local constant { ptr, ptr, ptr }, align 8
@_ZSt4cout = external global %"class.std::basic_ostream.29", align 8
@__orc_lcl..str.10.18 = external hidden constant [2 x i8], align 1

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_Z2hiii(i32 noundef, i32 noundef) #0

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5ColorC2EPKc(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5SceneC2E5ColorS0_d(ptr noundef nonnull align 8 dereferenceable(80), ptr noundef byval(%struct.Color.23) align 8, ptr noundef byval(%struct.Color.23) align 8, double noundef) #1 align 2

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znwm(i64 noundef) #2

declare i32 @__gxx_personality_v0(...)

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN10LambertianC2E5Color(ptr noundef nonnull align 8 dereferenceable(32), ptr noundef byval(%struct.Color.23) align 8) #3 align 2

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(ptr noundef) #4

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN10DielectricC2E5Colord(ptr noundef nonnull align 8 dereferenceable(40), ptr noundef byval(%struct.Color.23) align 8, double noundef) #3 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN8MetallicC2E5Colord(ptr noundef nonnull align 8 dereferenceable(40), ptr noundef byval(%struct.Color.23) align 8, double noundef) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZN5Scene8addShapeEP5Shape(ptr noundef nonnull align 8 dereferenceable(80), ptr noundef) #0 align 2

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #5

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN5PlaneC2EP8MaterialRK4Vec3S4_(ptr noundef nonnull align 8 dereferenceable(72), ptr noundef, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #3 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN6SphereC2EP8MaterialRK4Vec3d(ptr noundef nonnull align 8 dereferenceable(48), ptr noundef, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #1 align 2

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN6CameraC2ERK4Vec3jjjjS2_dS2_(ptr noundef nonnull align 8 dereferenceable(136), ptr noundef nonnull align 8 dereferenceable(24), i32 noundef, i32 noundef, i32 noundef, i32 noundef, ptr noundef nonnull align 8 dereferenceable(24), double noundef, ptr noundef nonnull align 8 dereferenceable(24)) #3 align 2

; Function Attrs: nounwind
declare i64 @clock() #6

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZN6Camera12captureSceneERK5Scene(ptr noundef nonnull align 8 dereferenceable(136), ptr noundef nonnull align 8 dereferenceable(80)) #0 align 2

declare noalias ptr @fopen(ptr noundef, ptr noundef) #7

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNK6Camera11developFilmEP8_IO_FILE(ptr noundef nonnull align 8 dereferenceable(136), ptr noundef) #8 align 2

declare i32 @fclose(ptr noundef) #7

; Function Attrs: nounwind
declare void @free(ptr noundef) #6

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN6CameraD2Ev(ptr noundef nonnull align 8 dereferenceable(136)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5SceneD2Ev(ptr noundef nonnull align 8 dereferenceable(80)) #1 align 2

; Function Attrs: nounwind
declare i32 @__isoc23_sscanf(ptr noundef, ptr noundef, ...) #6

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #9

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx114listIP5ShapeSaIS2_EEC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EEC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE10_List_implC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8__detail17_List_node_headerC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8__detail17_List_node_header7_M_initEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN8MaterialC2E5Color(ptr noundef nonnull align 8 dereferenceable(32), ptr noundef byval(%struct.Color.23) align 8) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK10Lambertian8interactERK3RayRK4Vec3S5_d(ptr noalias sret(%struct.Ray) align 8, ptr noundef nonnull align 8 dereferenceable(32), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24), double noundef) #0 align 2

declare void @__cxa_pure_virtual()

declare void @_ZplRK4Vec3S1_(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

declare void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #7

declare void @_Z16randomUnitVectorv(ptr sret(%struct.Vec3.24) align 8) #7

declare void @_ZmiRK4Vec3S1_(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN3RayC2ERK4Vec3S2_(ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #3 align 2

declare void @_ZNK4Vec39normalizeEv(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK5Color9transformERKSt8functionIFddEE(ptr noalias sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(32)) #0 align 2

; Function Attrs: nounwind
declare x86_fp80 @sqrtl(x86_fp80 noundef) #6

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8functionIFddEEC2IRDoFeeEvEEOT_(ptr noundef nonnull align 8 dereferenceable(32), ptr noundef nonnull) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8functionIFddEED2Ev(ptr noundef nonnull align 8 dereferenceable(32)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK10Dielectric8interactERK3RayRK4Vec3S5_d(ptr noalias sret(%struct.Ray) align 8, ptr noundef nonnull align 8 dereferenceable(40), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24), double noundef) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNKSt8functionIFddEEclEd(ptr noundef nonnull align 8 dereferenceable(32), double noundef) #0 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5ColorC2Eddd(ptr noundef nonnull align 8 dereferenceable(24), double noundef, double noundef, double noundef) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNKSt14_Function_base8_M_emptyEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noreturn
declare void @_ZSt25__throw_bad_function_callv() #10

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_baseC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNSt14_Function_base13_Base_managerIPDoFeeEE21_M_not_empty_functionIS1_EEbPT_(ptr noundef) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_base13_Base_managerIPDoFeeEE15_M_init_functorIRS1_EEvRSt9_Any_dataOT_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNSt17_Function_handlerIFddEPDoFeeEE9_M_invokeERKSt9_Any_dataOd(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef zeroext i1 @_ZNSt17_Function_handlerIFddEPDoFeeEE10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(16), i32 noundef) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_base13_Base_managerIPDoFeeEE9_M_createIRS1_EEvRSt9_Any_dataOT_St17integral_constantIbLb1EE(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull) #8 align 2

; Function Attrs: noinline noreturn nounwind uwtable
declare hidden void @__clang_call_terminate(ptr noundef) #11

declare ptr @__cxa_begin_catch(ptr)

declare void @_ZSt9terminatev()

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNSt9_Any_data9_M_accessEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZSt10__invoke_rIdRPDoFeeEJdEENSt9enable_ifIX16is_invocable_r_vIT_T0_DpT1_EES4_E4typeEOS5_DpOS6_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNSt14_Function_base13_Base_managerIPDoFeeEE14_M_get_pointerERKSt9_Any_data(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef x86_fp80 @_ZSt13__invoke_implIeRPDoFeeEJdEET_St14__invoke_otherOT0_DpOT1_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNKSt9_Any_data9_M_accessIPDoFeeEEERKT_v(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNKSt9_Any_data9_M_accessEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt9_Any_data9_M_accessIPKSt9type_infoEERT_v(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt9_Any_data9_M_accessIPPDoFeeEEERT_v(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef zeroext i1 @_ZNSt14_Function_base13_Base_managerIPDoFeeEE10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(16), i32 noundef) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_base13_Base_managerIPDoFeeEE15_M_init_functorIRKS2_EEvRSt9_Any_dataOT_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_base13_Base_managerIPDoFeeEE10_M_destroyERSt9_Any_dataSt17integral_constantIbLb1EE(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_base13_Base_managerIPDoFeeEE9_M_createIRKS2_EEvRSt9_Any_dataOT_St17integral_constantIbLb1EE(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt9_Any_data9_M_accessIPDoFeeEEERT_v(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Function_baseD2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

declare noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

declare void @_Z7refractRK4Vec3S1_d(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24), double noundef) #7

declare void @_ZngRK4Vec3(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNK4Vec36isZeroEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt25uniform_real_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(5000)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNK10Dielectric20schlickApproximationEdd(ptr noundef nonnull align 8 dereferenceable(40), double noundef, double noundef) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZSt3absd(double noundef) #8

declare void @_Z7reflectRK4Vec3S1_(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt25uniform_real_distributionIdEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEdRT_RKNS0_10param_typeE(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(5000), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEC2ERS2_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(5000)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZNSt8__detail8_AdaptorISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEdEclEv(ptr noundef nonnull align 8 dereferenceable(8)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNKSt25uniform_real_distributionIdE10param_type1bEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef double @_ZNKSt25uniform_real_distributionIdE10param_type1aEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #12

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef double @_ZSt18generate_canonicalIdLm53ESt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEET_RT1_(ptr noundef nonnull align 8 dereferenceable(5000)) #0

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3maxEv() #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE3minEv() #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef x86_fp80 @_ZSt3loge(x86_fp80 noundef) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3maxImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef i64 @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv(ptr noundef nonnull align 8 dereferenceable(5000)) #0 align 2

; Function Attrs: nounwind
declare double @nextafter(double noundef, double noundef) #6

; Function Attrs: nounwind
declare x86_fp80 @logl(x86_fp80 noundef) #6

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv(ptr noundef nonnull align 8 dereferenceable(5000)) #8 align 2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fabs.f64(double) #12

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK8Metallic8interactERK3RayRK4Vec3S5_d(ptr noalias sret(%struct.Ray) align 8, ptr noundef nonnull align 8 dereferenceable(40), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24), double noundef) #0 align 2

declare void @_ZmldRK4Vec3(ptr sret(%struct.Vec3.24) align 8, double noundef, ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNSt7__cxx114listIP5ShapeSaIS2_EE9push_backERKS2_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(8)) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNSt7__cxx114listIP5ShapeSaIS2_EE9_M_insertIJRKS2_EEEvSt14_List_iteratorIS2_EDpOT_(ptr noundef nonnull align 8 dereferenceable(24), ptr, ptr noundef nonnull align 8 dereferenceable(8)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNSt7__cxx114listIP5ShapeSaIS2_EE3endEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef ptr @_ZNSt7__cxx114listIP5ShapeSaIS2_EE14_M_create_nodeIJRKS2_EEEPSt10_List_nodeIS2_EDpOT_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(8)) #0 align 2

; Function Attrs: nounwind
declare void @_ZNSt8__detail15_List_node_base7_M_hookEPS0_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef) #6

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE11_M_inc_sizeEm(ptr noundef nonnull align 8 dereferenceable(24), i64 noundef) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef ptr @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE11_M_get_nodeEv(ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 1 dereferenceable(1) ptr @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE21_M_get_Node_allocatorEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__allocated_ptrISaISt10_List_nodeIP5ShapeEEEC2ERS4_PS3_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 1 dereferenceable(1), ptr noundef) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNSt10_List_nodeIP5ShapeE9_M_valptrEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(16) ptr @_ZNSt15__allocated_ptrISaISt10_List_nodeIP5ShapeEEEaSEDn(ptr noundef nonnull align 8 dereferenceable(16), ptr) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__allocated_ptrISaISt10_List_nodeIP5ShapeEEED2Ev(ptr noundef nonnull align 8 dereferenceable(16)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef ptr @_ZNSt15__new_allocatorISt10_List_nodeIP5ShapeEE8allocateEmPKv(ptr noundef nonnull align 1 dereferenceable(1), i64 noundef, ptr noundef) #0 align 2

; Function Attrs: noreturn
declare void @_ZSt28__throw_bad_array_new_lengthv() #10

; Function Attrs: noreturn
declare void @_ZSt17__throw_bad_allocv() #10

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZN9__gnu_cxx16__aligned_membufIP5ShapeE6_M_ptrEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZN9__gnu_cxx16__aligned_membufIP5ShapeE7_M_addrEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__new_allocatorISt10_List_nodeIP5ShapeEE10deallocateEPS3_m(ptr noundef nonnull align 1 dereferenceable(1), ptr noundef, i64 noundef) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_List_iteratorIP5ShapeEC2EPNSt8__detail15_List_node_baseE(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5ShapeC2EP8MaterialRK4Vec3(ptr noundef nonnull align 8 dereferenceable(40), ptr noundef, ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local { double, i8 } @_ZNK5Plane26computeNearestIntersectionERK3RayRK5Range(ptr noundef nonnull align 8 dereferenceable(72), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNK5Plane15computeNormalAtERK4Vec3(ptr noalias sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(72), ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8optionalIdEC2ESt9nullopt_t(ptr noundef nonnull align 8 dereferenceable(16)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNK5Range8containsERKd(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8optionalIdEC2IKdLb1EEEOT_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Optional_baseIdLb1ELb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(16)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt17_Optional_payloadIdLb1ELb1ELb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(9)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseIdEC2Ev(ptr noundef nonnull align 8 dereferenceable(9)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseIdE8_StorageIdLb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(8)) #1 align 2

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt14_Optional_baseIdLb1ELb1EEC2IJKdELb0EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 8 dereferenceable(8)) #3 align 2

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt17_Optional_payloadIdLb1ELb1ELb1EECI2St22_Optional_payload_baseIdEIJKdEEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(9), ptr noundef nonnull align 8 dereferenceable(8)) #3 align 2

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseIdEC2IJKdEEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(9), ptr noundef nonnull align 8 dereferenceable(8)) #3 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseIdE8_StorageIdLb1EEC2IJKdEEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local { double, i8 } @_ZNK6Sphere26computeNearestIntersectionERK3RayRK5Range(ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK6Sphere15computeNormalAtERK4Vec3(ptr noalias sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: nounwind
declare double @sqrt(double noundef) #6

; Function Attrs: nounwind
declare double @tan(double noundef) #6

declare void @_Z5crossRK4Vec3S1_(ptr sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare { i64, i1 } @llvm.umul.with.overflow.i64(i64, i64) #12

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znam(i64 noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZN5PixelC2Ehhh(ptr noundef nonnull align 1 dereferenceable(3), i8 noundef zeroext, i8 noundef zeroext, i8 noundef zeroext) #1 align 2

; Function Attrs: nobuiltin nounwind
declare void @_ZdaPv(ptr noundef) #4

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local i24 @_ZNK6Camera8getPixelERK5Scenejj(ptr noundef nonnull align 8 dereferenceable(136), ptr noundef nonnull align 8 dereferenceable(80), i32 noundef, i32 noundef) #0 align 2

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #7

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #7

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(ptr noundef nonnull align 8 dereferenceable(8)) #7

declare void @_ZplRK5ColorS1_(ptr sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK5Scene7castRayERK3RayRK5Rangej(ptr noalias sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(80), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(16), i32 noundef) #0 align 2

declare void @_ZdvRK5Colord(ptr sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #7

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZN5PixelC2ERK5Color(ptr noundef nonnull align 1 dereferenceable(3), ptr noundef nonnull align 8 dereferenceable(24)) #3 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx114listI5ColorSaIS1_EEC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNKSt7__cxx114listI5ColorSaIS1_EE4sizeEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK5Scene11findNearestERK3RayRK5Range(ptr noalias sret(%"class.std::optional.40") align 8, ptr noundef nonnull align 8 dereferenceable(80), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNKSt8optionalI12IntersectionEcvbEv(ptr noundef nonnull align 8 dereferenceable(72)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNSt7__cxx114listI5ColorSaIS1_EE9push_backERKS1_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNSt8optionalI12IntersectionEptEv(ptr noundef nonnull align 8 dereferenceable(72)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK5Scene6skyBoxERK4Vec3(ptr noalias sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(80), ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNSt7__cxx114listI5ColorSaIS1_EE5beginEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNSt7__cxx114listI5ColorSaIS1_EE3endEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZStneRKSt14_List_iteratorI5ColorES3_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(24) ptr @_ZNKSt14_List_iteratorI5ColorEdeEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

declare void @_ZmlRK5ColorS1_(ptr sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #7

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt14_List_iteratorI5ColorEppEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx114listI5ColorSaIS1_EED2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EEC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE10_List_implC2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef i64 @_ZNKSt7__cxx114listI5ColorSaIS1_EE13_M_node_countEv(ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef i64 @_ZNKSt7__cxx1110_List_baseI5ColorSaIS1_EE11_M_get_sizeEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8optionalI12IntersectionEC2ESt9nullopt_t(ptr noundef nonnull align 8 dereferenceable(72)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNKSt7__cxx114listIP5ShapeSaIS2_EE5beginEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNKSt7__cxx114listIP5ShapeSaIS2_EE3endEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZStneRKSt20_List_const_iteratorIP5ShapeES4_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNKSt20_List_const_iteratorIP5ShapeEdeEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZN5Shape12intersectRayERK3RayRK5Range(ptr noalias sret(%"class.std::optional.40") align 8, ptr noundef nonnull align 8 dereferenceable(40), ptr noundef nonnull align 8 dereferenceable(48), ptr noundef nonnull align 8 dereferenceable(16)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt20_List_const_iteratorIP5ShapeEppEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_Optional_baseI12IntersectionLb1ELb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(72)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt17_Optional_payloadI12IntersectionLb1ELb1ELb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(65)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZNSt22_Optional_payload_baseI12IntersectionEC2Ev(ptr noundef nonnull align 8 dereferenceable(65) %this) #1 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_payload = getelementptr inbounds %"struct.std::_Optional_payload_base.44", ptr %this1, i32 0, i32 0
  call void @_ZNSt22_Optional_payload_baseI12IntersectionE8_StorageIS0_Lb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(64) %_M_payload) #13
  %_M_engaged = getelementptr inbounds %"struct.std::_Optional_payload_base.44", ptr %this1, i32 0, i32 1
  store i8 0, ptr %_M_engaged, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseI12IntersectionE8_StorageIS0_Lb1EEC2Ev(ptr noundef nonnull align 8 dereferenceable(64)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt20_List_const_iteratorIP5ShapeEC2EPKNSt8__detail15_List_node_baseE(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNKSt10_List_nodeIP5ShapeE9_M_valptrEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNK9__gnu_cxx16__aligned_membufIP5ShapeE6_M_ptrEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define linkonce_odr dso_local noundef ptr @_ZNK9__gnu_cxx16__aligned_membufIP5ShapeE7_M_addrEv(ptr noundef nonnull align 8 dereferenceable(8) %this) #8 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_storage = getelementptr inbounds %"struct.__gnu_cxx::__aligned_membuf", ptr %this1, i32 0, i32 0
  ret ptr %_M_storage
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNKSt8optionalIdEcvbEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNK3Ray7projectEd(ptr noalias sret(%struct.Vec3.24) align 8, ptr noundef nonnull align 8 dereferenceable(48), double noundef) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNRSt8optionalIdEdeEv(ptr noundef nonnull align 8 dereferenceable(16)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt8optionalI12IntersectionEC2IS0_Lb1EEEOT_(ptr noundef nonnull align 8 dereferenceable(72), ptr noundef nonnull align 8 dereferenceable(64)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNKSt19_Optional_base_implIdSt14_Optional_baseIdLb1ELb1EEE13_M_is_engagedEv(ptr noundef nonnull align 1 dereferenceable(1)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt19_Optional_base_implIdSt14_Optional_baseIdLb1ELb1EEE6_M_getEv(ptr noundef nonnull align 1 dereferenceable(1)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define linkonce_odr dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt22_Optional_payload_baseIdE6_M_getEv(ptr noundef nonnull align 8 dereferenceable(9) %this) #8 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %_M_payload = getelementptr inbounds %"struct.std::_Optional_payload_base", ptr %this1, i32 0, i32 0
  ret ptr %_M_payload
}

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt14_Optional_baseI12IntersectionLb1ELb1EEC2IJS0_ELb0EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(72), ptr noundef nonnull align 8 dereferenceable(64)) #3 align 2

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local void @_ZNSt17_Optional_payloadI12IntersectionLb1ELb1ELb1EECI2St22_Optional_payload_baseIS0_EIJS0_EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(65) %this, ptr noundef nonnull align 8 dereferenceable(64) %0) #3 comdat align 2 {
entry:
  %1 = alloca %"struct.std::in_place_t", align 1
  %this.addr = alloca ptr, align 8
  %.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %0, ptr %.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %2 = load ptr, ptr %.addr, align 8
  call void @_ZNSt22_Optional_payload_baseI12IntersectionEC2IJS0_EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(65) %this1, ptr noundef nonnull align 8 dereferenceable(64) %2)
  ret void
}

; Function Attrs: noinline optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseI12IntersectionEC2IJS0_EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(65), ptr noundef nonnull align 8 dereferenceable(64)) #3 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt22_Optional_payload_baseI12IntersectionE8_StorageIS0_Lb1EEC2IJS0_EEESt10in_place_tDpOT_(ptr noundef nonnull align 8 dereferenceable(64), ptr noundef nonnull align 8 dereferenceable(64)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZNKSt19_Optional_base_implI12IntersectionSt14_Optional_baseIS0_Lb1ELb1EEE13_M_is_engagedEv(ptr noundef nonnull align 1 dereferenceable(1)) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local void @_ZNSt7__cxx114listI5ColorSaIS1_EE9_M_insertIJRKS1_EEEvSt14_List_iteratorIS1_EDpOT_(ptr noundef nonnull align 8 dereferenceable(24), ptr, ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef ptr @_ZNSt7__cxx114listI5ColorSaIS1_EE14_M_create_nodeIJRKS1_EEEPSt10_List_nodeIS1_EDpOT_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE11_M_inc_sizeEm(ptr noundef nonnull align 8 dereferenceable(24), i64 noundef) #8 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef ptr @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE11_M_get_nodeEv(ptr noundef nonnull align 8 dereferenceable(24)) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 1 dereferenceable(1) ptr @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE21_M_get_Node_allocatorEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__allocated_ptrISaISt10_List_nodeI5ColorEEEC2ERS3_PS2_(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef nonnull align 1 dereferenceable(1), ptr noundef) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZNSt10_List_nodeI5ColorE9_M_valptrEv(ptr noundef nonnull align 8 dereferenceable(40)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(16) ptr @_ZNSt15__allocated_ptrISaISt10_List_nodeI5ColorEEEaSEDn(ptr noundef nonnull align 8 dereferenceable(16), ptr) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__allocated_ptrISaISt10_List_nodeI5ColorEEED2Ev(ptr noundef nonnull align 8 dereferenceable(16)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
define linkonce_odr dso_local noundef ptr @_ZNSt15__new_allocatorISt10_List_nodeI5ColorEE8allocateEmPKv(ptr noundef nonnull align 1 dereferenceable(1) %this, i64 noundef %__n, ptr noundef %0) #0 comdat align 2 {
entry:
  %this.addr.i = alloca ptr, align 8
  %this.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  %.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  store ptr %0, ptr %.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %1 = load i64, ptr %__n.addr, align 8
  store ptr %this1, ptr %this.addr.i, align 8
  %this1.i = load ptr, ptr %this.addr.i, align 8
  %cmp = icmp ugt i64 %1, 230584300921369395
  br i1 %cmp, label %if.then, label %if.end4

if.then:                                          ; preds = %entry
  %2 = load i64, ptr %__n.addr, align 8
  %cmp2 = icmp ugt i64 %2, 461168601842738790
  br i1 %cmp2, label %if.then3, label %if.end

if.then3:                                         ; preds = %if.then
  call void @_ZSt28__throw_bad_array_new_lengthv() #14
  unreachable

if.end:                                           ; preds = %if.then
  call void @_ZSt17__throw_bad_allocv() #14
  unreachable

if.end4:                                          ; preds = %entry
  %3 = load i64, ptr %__n.addr, align 8
  %mul = mul i64 %3, 40
  %call5 = call noalias noundef nonnull ptr @_Znwm(i64 noundef %mul) #15
  ret ptr %call5
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZN9__gnu_cxx16__aligned_membufI5ColorE6_M_ptrEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef ptr @_ZN9__gnu_cxx16__aligned_membufI5ColorE7_M_addrEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZNSt15__new_allocatorISt10_List_nodeI5ColorEE10deallocateEPS2_m(ptr noundef nonnull align 1 dereferenceable(1) %this, ptr noundef %__p, i64 noundef %__n) #8 comdat align 2 {
entry:
  %this.addr = alloca ptr, align 8
  %__p.addr = alloca ptr, align 8
  %__n.addr = alloca i64, align 8
  store ptr %this, ptr %this.addr, align 8
  store ptr %__p, ptr %__p.addr, align 8
  store i64 %__n, ptr %__n.addr, align 8
  %this1 = load ptr, ptr %this.addr, align 8
  %0 = load ptr, ptr %__p.addr, align 8
  call void @_ZdlPv(ptr noundef %0) #16
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(64) ptr @_ZNSt19_Optional_base_implI12IntersectionSt14_Optional_baseIS0_Lb1ELb1EEE6_M_getEv(ptr noundef nonnull align 1 dereferenceable(1)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(64) ptr @_ZNSt22_Optional_payload_baseI12IntersectionE6_M_getEv(ptr noundef nonnull align 8 dereferenceable(65)) #8 align 2

declare void @_ZmlRK5Colord(ptr sret(%struct.Color.23) align 8, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #7

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt14_List_iteratorI5ColorEC2EPNSt8__detail15_List_node_baseE(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EED2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE8_M_clearEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE10_List_implD2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseI5ColorSaIS1_EE11_M_put_nodeEPSt10_List_nodeIS1_E(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__new_allocatorISt10_List_nodeI5ColorEED2Ev(ptr noundef nonnull align 1 dereferenceable(1)) #1 align 2

; Function Attrs: mustprogress noinline optnone uwtable
declare dso_local noundef zeroext i8 @_ZN5Pixel5clampEd(double noundef) #0 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 4 dereferenceable(4) ptr @_ZSt3minIiERKT_S2_S2_(ptr noundef nonnull align 4 dereferenceable(4), ptr noundef nonnull align 4 dereferenceable(4)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 4 dereferenceable(4) ptr @_ZSt3maxIiERKT_S2_S2_(ptr noundef nonnull align 4 dereferenceable(4), ptr noundef nonnull align 4 dereferenceable(4)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local ptr @_ZNSt7__cxx114listIP5ShapeSaIS2_EE5beginEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef zeroext i1 @_ZStneRKSt14_List_iteratorIP5ShapeES4_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef nonnull align 8 dereferenceable(8)) #8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNKSt14_List_iteratorIP5ShapeEdeEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZNSt14_List_iteratorIP5ShapeEppEv(ptr noundef nonnull align 8 dereferenceable(8)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx114listIP5ShapeSaIS2_EED2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EED2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE8_M_clearEv(ptr noundef nonnull align 8 dereferenceable(24)) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE10_List_implD2Ev(ptr noundef nonnull align 8 dereferenceable(24)) #1 align 2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
declare dso_local void @_ZNSt7__cxx1110_List_baseIP5ShapeSaIS2_EE11_M_put_nodeEPSt10_List_nodeIS2_E(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef) #8 align 2

; Function Attrs: noinline nounwind optnone uwtable
declare dso_local void @_ZNSt15__new_allocatorISt10_List_nodeIP5ShapeEED2Ev(ptr noundef nonnull align 1 dereferenceable(1)) #1 align 2

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nobuiltin allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nobuiltin nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #6 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #10 = { noreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { noinline noreturn nounwind uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #12 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #13 = { nounwind }
attributes #14 = { noreturn }
attributes #15 = { builtin allocsize(0) }
attributes #16 = { builtin nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
