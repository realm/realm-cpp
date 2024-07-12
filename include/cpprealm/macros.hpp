////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_MACROS_HPP
#define CPPREALM_MACROS_HPP

#define FE_0(WHAT, cls)
#define FE_1(WHAT, cls, X) WHAT(cls, X)
#define FE_2(WHAT, cls, X, ...) WHAT(cls, X)FE_1(WHAT, cls, __VA_ARGS__)
#define FE_3(WHAT, cls, X, ...) WHAT(cls, X)FE_2(WHAT, cls, __VA_ARGS__)
#define FE_4(WHAT, cls, X, ...) WHAT(cls, X)FE_3(WHAT, cls, __VA_ARGS__)
#define FE_5(WHAT, cls, X, ...) WHAT(cls, X)FE_4(WHAT, cls, __VA_ARGS__)
#define FE_6(WHAT, cls, X, ...) WHAT(cls, X)FE_5(WHAT, cls, __VA_ARGS__)
#define FE_7(WHAT, cls, X, ...) WHAT(cls, X)FE_6(WHAT, cls, __VA_ARGS__)
#define FE_8(WHAT, cls, X, ...) WHAT(cls, X)FE_7(WHAT, cls, __VA_ARGS__)
#define FE_9(WHAT, cls, X, ...) WHAT(cls, X)FE_8(WHAT, cls, __VA_ARGS__)
#define FE_10(WHAT, cls, X, ...) WHAT(cls, X)FE_9(WHAT, cls, __VA_ARGS__)
#define FE_11(WHAT, cls, X, ...) WHAT(cls, X)FE_10(WHAT, cls, __VA_ARGS__)
#define FE_12(WHAT, cls, X, ...) WHAT(cls, X)FE_11(WHAT, cls, __VA_ARGS__)
#define FE_13(WHAT, cls, X, ...) WHAT(cls, X)FE_12(WHAT, cls, __VA_ARGS__)
#define FE_14(WHAT, cls, X, ...) WHAT(cls, X)FE_13(WHAT, cls, __VA_ARGS__)
#define FE_15(WHAT, cls, X, ...) WHAT(cls, X)FE_14(WHAT, cls, __VA_ARGS__)
#define FE_16(WHAT, cls, X, ...) WHAT(cls, X)FE_15(WHAT, cls, __VA_ARGS__)
#define FE_17(WHAT, cls, X, ...) WHAT(cls, X)FE_16(WHAT, cls, __VA_ARGS__)
#define FE_18(WHAT, cls, X, ...) WHAT(cls, X)FE_17(WHAT, cls, __VA_ARGS__)
#define FE_19(WHAT, cls, X, ...) WHAT(cls, X)FE_18(WHAT, cls, __VA_ARGS__)
#define FE_20(WHAT, cls, X, ...) WHAT(cls, X)FE_19(WHAT, cls, __VA_ARGS__)
#define FE_21(WHAT, cls, X, ...) WHAT(cls, X)FE_20(WHAT, cls, __VA_ARGS__)
#define FE_22(WHAT, cls, X, ...) WHAT(cls, X)FE_21(WHAT, cls, __VA_ARGS__)
#define FE_23(WHAT, cls, X, ...) WHAT(cls, X)FE_22(WHAT, cls, __VA_ARGS__)
#define FE_24(WHAT, cls, X, ...) WHAT(cls, X)FE_23(WHAT, cls, __VA_ARGS__)
#define FE_25(WHAT, cls, X, ...) WHAT(cls, X)FE_24(WHAT, cls, __VA_ARGS__)
#define FE_26(WHAT, cls, X, ...) WHAT(cls, X)FE_25(WHAT, cls, __VA_ARGS__)
#define FE_27(WHAT, cls, X, ...) WHAT(cls, X)FE_26(WHAT, cls, __VA_ARGS__)
#define FE_28(WHAT, cls, X, ...) WHAT(cls, X)FE_27(WHAT, cls, __VA_ARGS__)
#define FE_29(WHAT, cls, X, ...) WHAT(cls, X)FE_28(WHAT, cls, __VA_ARGS__)
#define FE_30(WHAT, cls, X, ...) WHAT(cls, X)FE_29(WHAT, cls, __VA_ARGS__)
#define FE_31(WHAT, cls, X, ...) WHAT(cls, X)FE_30(WHAT, cls, __VA_ARGS__)
#define FE_32(WHAT, cls, X, ...) WHAT(cls, X)FE_31(WHAT, cls, __VA_ARGS__)
#define FE_33(WHAT, cls, X, ...) WHAT(cls, X)FE_32(WHAT, cls, __VA_ARGS__)
#define FE_34(WHAT, cls, X, ...) WHAT(cls, X)FE_33(WHAT, cls, __VA_ARGS__)
#define FE_35(WHAT, cls, X, ...) WHAT(cls, X)FE_34(WHAT, cls, __VA_ARGS__)
#define FE_36(WHAT, cls, X, ...) WHAT(cls, X)FE_35(WHAT, cls, __VA_ARGS__)
#define FE_37(WHAT, cls, X, ...) WHAT(cls, X)FE_36(WHAT, cls, __VA_ARGS__)
#define FE_38(WHAT, cls, X, ...) WHAT(cls, X)FE_37(WHAT, cls, __VA_ARGS__)
#define FE_39(WHAT, cls, X, ...) WHAT(cls, X)FE_38(WHAT, cls, __VA_ARGS__)
#define FE_40(WHAT, cls, X, ...) WHAT(cls, X)FE_39(WHAT, cls, __VA_ARGS__)
#define FE_41(WHAT, cls, X, ...) WHAT(cls, X)FE_40(WHAT, cls, __VA_ARGS__)
#define FE_42(WHAT, cls, X, ...) WHAT(cls, X)FE_41(WHAT, cls, __VA_ARGS__)
#define FE_43(WHAT, cls, X, ...) WHAT(cls, X)FE_42(WHAT, cls, __VA_ARGS__)
#define FE_44(WHAT, cls, X, ...) WHAT(cls, X)FE_43(WHAT, cls, __VA_ARGS__)
#define FE_45(WHAT, cls, X, ...) WHAT(cls, X)FE_44(WHAT, cls, __VA_ARGS__)
#define FE_46(WHAT, cls, X, ...) WHAT(cls, X)FE_45(WHAT, cls, __VA_ARGS__)
#define FE_47(WHAT, cls, X, ...) WHAT(cls, X)FE_46(WHAT, cls, __VA_ARGS__)
#define FE_48(WHAT, cls, X, ...) WHAT(cls, X)FE_47(WHAT, cls, __VA_ARGS__)
#define FE_49(WHAT, cls, X, ...) WHAT(cls, X)FE_48(WHAT, cls, __VA_ARGS__)
#define FE_50(WHAT, cls, X, ...) WHAT(cls, X)FE_49(WHAT, cls, __VA_ARGS__)
#define FE_51(WHAT, cls, X, ...) WHAT(cls, X)FE_50(WHAT, cls, __VA_ARGS__)
#define FE_52(WHAT, cls, X, ...) WHAT(cls, X)FE_51(WHAT, cls, __VA_ARGS__)
#define FE_53(WHAT, cls, X, ...) WHAT(cls, X)FE_52(WHAT, cls, __VA_ARGS__)
#define FE_54(WHAT, cls, X, ...) WHAT(cls, X)FE_53(WHAT, cls, __VA_ARGS__)
#define FE_55(WHAT, cls, X, ...) WHAT(cls, X)FE_54(WHAT, cls, __VA_ARGS__)
#define FE_56(WHAT, cls, X, ...) WHAT(cls, X)FE_55(WHAT, cls, __VA_ARGS__)
#define FE_57(WHAT, cls, X, ...) WHAT(cls, X)FE_56(WHAT, cls, __VA_ARGS__)
#define FE_58(WHAT, cls, X, ...) WHAT(cls, X)FE_57(WHAT, cls, __VA_ARGS__)
#define FE_59(WHAT, cls, X, ...) WHAT(cls, X)FE_58(WHAT, cls, __VA_ARGS__)
#define FE_60(WHAT, cls, X, ...) WHAT(cls, X)FE_59(WHAT, cls, __VA_ARGS__)
#define FE_61(WHAT, cls, X, ...) WHAT(cls, X)FE_60(WHAT, cls, __VA_ARGS__)
#define FE_62(WHAT, cls, X, ...) WHAT(cls, X)FE_61(WHAT, cls, __VA_ARGS__)
#define FE_63(WHAT, cls, X, ...) WHAT(cls, X)FE_62(WHAT, cls, __VA_ARGS__)
#define FE_64(WHAT, cls, X, ...) WHAT(cls, X)FE_63(WHAT, cls, __VA_ARGS__)
#define FE_65(WHAT, cls, X, ...) WHAT(cls, X)FE_64(WHAT, cls, __VA_ARGS__)
#define FE_66(WHAT, cls, X, ...) WHAT(cls, X)FE_65(WHAT, cls, __VA_ARGS__)
#define FE_67(WHAT, cls, X, ...) WHAT(cls, X)FE_66(WHAT, cls, __VA_ARGS__)
#define FE_68(WHAT, cls, X, ...) WHAT(cls, X)FE_67(WHAT, cls, __VA_ARGS__)
#define FE_69(WHAT, cls, X, ...) WHAT(cls, X)FE_68(WHAT, cls, __VA_ARGS__)
#define FE_70(WHAT, cls, X, ...) WHAT(cls, X)FE_69(WHAT, cls, __VA_ARGS__)
#define FE_71(WHAT, cls, X, ...) WHAT(cls, X)FE_70(WHAT, cls, __VA_ARGS__)
#define FE_72(WHAT, cls, X, ...) WHAT(cls, X)FE_71(WHAT, cls, __VA_ARGS__)
#define FE_73(WHAT, cls, X, ...) WHAT(cls, X)FE_72(WHAT, cls, __VA_ARGS__)
#define FE_74(WHAT, cls, X, ...) WHAT(cls, X)FE_73(WHAT, cls, __VA_ARGS__)
#define FE_75(WHAT, cls, X, ...) WHAT(cls, X)FE_74(WHAT, cls, __VA_ARGS__)
#define FE_76(WHAT, cls, X, ...) WHAT(cls, X)FE_75(WHAT, cls, __VA_ARGS__)
#define FE_77(WHAT, cls, X, ...) WHAT(cls, X)FE_76(WHAT, cls, __VA_ARGS__)
#define FE_78(WHAT, cls, X, ...) WHAT(cls, X)FE_77(WHAT, cls, __VA_ARGS__)
#define FE_79(WHAT, cls, X, ...) WHAT(cls, X)FE_78(WHAT, cls, __VA_ARGS__)
#define FE_80(WHAT, cls, X, ...) WHAT(cls, X)FE_79(WHAT, cls, __VA_ARGS__)
#define FE_81(WHAT, cls, X, ...) WHAT(cls, X)FE_80(WHAT, cls, __VA_ARGS__)
#define FE_82(WHAT, cls, X, ...) WHAT(cls, X)FE_81(WHAT, cls, __VA_ARGS__)
#define FE_83(WHAT, cls, X, ...) WHAT(cls, X)FE_82(WHAT, cls, __VA_ARGS__)
#define FE_84(WHAT, cls, X, ...) WHAT(cls, X)FE_83(WHAT, cls, __VA_ARGS__)
#define FE_85(WHAT, cls, X, ...) WHAT(cls, X)FE_84(WHAT, cls, __VA_ARGS__)
#define FE_86(WHAT, cls, X, ...) WHAT(cls, X)FE_85(WHAT, cls, __VA_ARGS__)
#define FE_87(WHAT, cls, X, ...) WHAT(cls, X)FE_86(WHAT, cls, __VA_ARGS__)
#define FE_88(WHAT, cls, X, ...) WHAT(cls, X)FE_87(WHAT, cls, __VA_ARGS__)
#define FE_89(WHAT, cls, X, ...) WHAT(cls, X)FE_88(WHAT, cls, __VA_ARGS__)
#define FE_90(WHAT, cls, X, ...) WHAT(cls, X)FE_89(WHAT, cls, __VA_ARGS__)
#define FE_91(WHAT, cls, X, ...) WHAT(cls, X)FE_90(WHAT, cls, __VA_ARGS__)
#define FE_92(WHAT, cls, X, ...) WHAT(cls, X)FE_91(WHAT, cls, __VA_ARGS__)
#define FE_93(WHAT, cls, X, ...) WHAT(cls, X)FE_92(WHAT, cls, __VA_ARGS__)
#define FE_94(WHAT, cls, X, ...) WHAT(cls, X)FE_93(WHAT, cls, __VA_ARGS__)
#define FE_95(WHAT, cls, X, ...) WHAT(cls, X)FE_94(WHAT, cls, __VA_ARGS__)
#define FE_96(WHAT, cls, X, ...) WHAT(cls, X)FE_95(WHAT, cls, __VA_ARGS__)
#define FE_97(WHAT, cls, X, ...) WHAT(cls, X)FE_96(WHAT, cls, __VA_ARGS__)
#define FE_98(WHAT, cls, X, ...) WHAT(cls, X)FE_97(WHAT, cls, __VA_ARGS__)
#define FE_99(WHAT, cls, X, ...) WHAT(cls, X)FE_98(WHAT, cls, __VA_ARGS__)
#define FE_100(WHAT, cls, X, ...) WHAT(cls, X) FE_99(WHAT, cls, __VA_ARGS__)
#define FE_101(WHAT, cls, X, ...) WHAT(cls, X) FE_100(WHAT, cls, __VA_ARGS__)
#define FE_102(WHAT, cls, X, ...) WHAT(cls, X) FE_101(WHAT, cls, __VA_ARGS__)
#define FE_103(WHAT, cls, X, ...) WHAT(cls, X) FE_102(WHAT, cls, __VA_ARGS__)
#define FE_104(WHAT, cls, X, ...) WHAT(cls, X) FE_103(WHAT, cls, __VA_ARGS__)
#define FE_105(WHAT, cls, X, ...) WHAT(cls, X) FE_104(WHAT, cls, __VA_ARGS__)
#define FE_106(WHAT, cls, X, ...) WHAT(cls, X) FE_105(WHAT, cls, __VA_ARGS__)
#define FE_107(WHAT, cls, X, ...) WHAT(cls, X) FE_106(WHAT, cls, __VA_ARGS__)
#define FE_108(WHAT, cls, X, ...) WHAT(cls, X) FE_107(WHAT, cls, __VA_ARGS__)
#define FE_109(WHAT, cls, X, ...) WHAT(cls, X) FE_108(WHAT, cls, __VA_ARGS__)
#define FE_110(WHAT, cls, X, ...) WHAT(cls, X) FE_109(WHAT, cls, __VA_ARGS__)
#define FE_111(WHAT, cls, X, ...) WHAT(cls, X) FE_110(WHAT, cls, __VA_ARGS__)
#define FE_112(WHAT, cls, X, ...) WHAT(cls, X) FE_111(WHAT, cls, __VA_ARGS__)
#define FE_113(WHAT, cls, X, ...) WHAT(cls, X) FE_112(WHAT, cls, __VA_ARGS__)
#define FE_114(WHAT, cls, X, ...) WHAT(cls, X) FE_113(WHAT, cls, __VA_ARGS__)
#define FE_115(WHAT, cls, X, ...) WHAT(cls, X) FE_114(WHAT, cls, __VA_ARGS__)
#define FE_116(WHAT, cls, X, ...) WHAT(cls, X) FE_115(WHAT, cls, __VA_ARGS__)
#define FE_117(WHAT, cls, X, ...) WHAT(cls, X) FE_116(WHAT, cls, __VA_ARGS__)
#define FE_118(WHAT, cls, X, ...) WHAT(cls, X) FE_117(WHAT, cls, __VA_ARGS__)
#define FE_119(WHAT, cls, X, ...) WHAT(cls, X) FE_118(WHAT, cls, __VA_ARGS__)
#define FE_120(WHAT, cls, X, ...) WHAT(cls, X) FE_119(WHAT, cls, __VA_ARGS__)
#define FE_121(WHAT, cls, X, ...) WHAT(cls, X) FE_120(WHAT, cls, __VA_ARGS__)
#define FE_122(WHAT, cls, X, ...) WHAT(cls, X) FE_121(WHAT, cls, __VA_ARGS__)
#define FE_123(WHAT, cls, X, ...) WHAT(cls, X) FE_122(WHAT, cls, __VA_ARGS__)
#define FE_124(WHAT, cls, X, ...) WHAT(cls, X) FE_123(WHAT, cls, __VA_ARGS__)
#define FE_125(WHAT, cls, X, ...) WHAT(cls, X) FE_124(WHAT, cls, __VA_ARGS__)
#define FE_126(WHAT, cls, X, ...) WHAT(cls, X) FE_125(WHAT, cls, __VA_ARGS__)
#define FE_127(WHAT, cls, X, ...) WHAT(cls, X) FE_126(WHAT, cls, __VA_ARGS__)
#define FE_128(WHAT, cls, X, ...) WHAT(cls, X) FE_127(WHAT, cls, __VA_ARGS__)
#define FE_129(WHAT, cls, X, ...) WHAT(cls, X) FE_128(WHAT, cls, __VA_ARGS__)
#define FE_130(WHAT, cls, X, ...) WHAT(cls, X) FE_129(WHAT, cls, __VA_ARGS__)
#define FE_131(WHAT, cls, X, ...) WHAT(cls, X) FE_130(WHAT, cls, __VA_ARGS__)
#define FE_132(WHAT, cls, X, ...) WHAT(cls, X) FE_131(WHAT, cls, __VA_ARGS__)
#define FE_133(WHAT, cls, X, ...) WHAT(cls, X) FE_132(WHAT, cls, __VA_ARGS__)
#define FE_134(WHAT, cls, X, ...) WHAT(cls, X) FE_133(WHAT, cls, __VA_ARGS__)
#define FE_135(WHAT, cls, X, ...) WHAT(cls, X) FE_134(WHAT, cls, __VA_ARGS__)
#define FE_136(WHAT, cls, X, ...) WHAT(cls, X) FE_135(WHAT, cls, __VA_ARGS__)
#define FE_137(WHAT, cls, X, ...) WHAT(cls, X) FE_136(WHAT, cls, __VA_ARGS__)
#define FE_138(WHAT, cls, X, ...) WHAT(cls, X) FE_137(WHAT, cls, __VA_ARGS__)
#define FE_139(WHAT, cls, X, ...) WHAT(cls, X) FE_138(WHAT, cls, __VA_ARGS__)
#define FE_140(WHAT, cls, X, ...) WHAT(cls, X) FE_139(WHAT, cls, __VA_ARGS__)
#define FE_141(WHAT, cls, X, ...) WHAT(cls, X) FE_140(WHAT, cls, __VA_ARGS__)
#define FE_142(WHAT, cls, X, ...) WHAT(cls, X) FE_141(WHAT, cls, __VA_ARGS__)
#define FE_143(WHAT, cls, X, ...) WHAT(cls, X) FE_142(WHAT, cls, __VA_ARGS__)
#define FE_144(WHAT, cls, X, ...) WHAT(cls, X) FE_143(WHAT, cls, __VA_ARGS__)
#define FE_145(WHAT, cls, X, ...) WHAT(cls, X) FE_144(WHAT, cls, __VA_ARGS__)
#define FE_146(WHAT, cls, X, ...) WHAT(cls, X) FE_145(WHAT, cls, __VA_ARGS__)
#define FE_147(WHAT, cls, X, ...) WHAT(cls, X) FE_146(WHAT, cls, __VA_ARGS__)
#define FE_148(WHAT, cls, X, ...) WHAT(cls, X) FE_147(WHAT, cls, __VA_ARGS__)
#define FE_149(WHAT, cls, X, ...) WHAT(cls, X) FE_148(WHAT, cls, __VA_ARGS__)
#define FE_150(WHAT, cls, X, ...) WHAT(cls, X) FE_149(WHAT, cls, __VA_ARGS__)
#define FE_151(WHAT, cls, X, ...) WHAT(cls, X) FE_150(WHAT, cls, __VA_ARGS__)
#define FE_152(WHAT, cls, X, ...) WHAT(cls, X) FE_151(WHAT, cls, __VA_ARGS__)
#define FE_153(WHAT, cls, X, ...) WHAT(cls, X) FE_152(WHAT, cls, __VA_ARGS__)
#define FE_154(WHAT, cls, X, ...) WHAT(cls, X) FE_153(WHAT, cls, __VA_ARGS__)
#define FE_155(WHAT, cls, X, ...) WHAT(cls, X) FE_154(WHAT, cls, __VA_ARGS__)
#define FE_156(WHAT, cls, X, ...) WHAT(cls, X) FE_155(WHAT, cls, __VA_ARGS__)
#define FE_157(WHAT, cls, X, ...) WHAT(cls, X) FE_156(WHAT, cls, __VA_ARGS__)
#define FE_158(WHAT, cls, X, ...) WHAT(cls, X) FE_157(WHAT, cls, __VA_ARGS__)
#define FE_159(WHAT, cls, X, ...) WHAT(cls, X) FE_158(WHAT, cls, __VA_ARGS__)
#define FE_160(WHAT, cls, X, ...) WHAT(cls, X) FE_159(WHAT, cls, __VA_ARGS__)
#define FE_161(WHAT, cls, X, ...) WHAT(cls, X) FE_160(WHAT, cls, __VA_ARGS__)
#define FE_162(WHAT, cls, X, ...) WHAT(cls, X) FE_161(WHAT, cls, __VA_ARGS__)
#define FE_163(WHAT, cls, X, ...) WHAT(cls, X) FE_162(WHAT, cls, __VA_ARGS__)
#define FE_164(WHAT, cls, X, ...) WHAT(cls, X) FE_163(WHAT, cls, __VA_ARGS__)
#define FE_165(WHAT, cls, X, ...) WHAT(cls, X) FE_164(WHAT, cls, __VA_ARGS__)
#define FE_166(WHAT, cls, X, ...) WHAT(cls, X) FE_165(WHAT, cls, __VA_ARGS__)
#define FE_167(WHAT, cls, X, ...) WHAT(cls, X) FE_166(WHAT, cls, __VA_ARGS__)
#define FE_168(WHAT, cls, X, ...) WHAT(cls, X) FE_167(WHAT, cls, __VA_ARGS__)
#define FE_169(WHAT, cls, X, ...) WHAT(cls, X) FE_168(WHAT, cls, __VA_ARGS__)
#define FE_170(WHAT, cls, X, ...) WHAT(cls, X) FE_169(WHAT, cls, __VA_ARGS__)
#define FE_171(WHAT, cls, X, ...) WHAT(cls, X) FE_170(WHAT, cls, __VA_ARGS__)
#define FE_172(WHAT, cls, X, ...) WHAT(cls, X) FE_171(WHAT, cls, __VA_ARGS__)
#define FE_173(WHAT, cls, X, ...) WHAT(cls, X) FE_172(WHAT, cls, __VA_ARGS__)
#define FE_174(WHAT, cls, X, ...) WHAT(cls, X) FE_173(WHAT, cls, __VA_ARGS__)
#define FE_175(WHAT, cls, X, ...) WHAT(cls, X) FE_174(WHAT, cls, __VA_ARGS__)
#define FE_176(WHAT, cls, X, ...) WHAT(cls, X) FE_175(WHAT, cls, __VA_ARGS__)
#define FE_177(WHAT, cls, X, ...) WHAT(cls, X) FE_176(WHAT, cls, __VA_ARGS__)
#define FE_178(WHAT, cls, X, ...) WHAT(cls, X) FE_177(WHAT, cls, __VA_ARGS__)
#define FE_179(WHAT, cls, X, ...) WHAT(cls, X) FE_178(WHAT, cls, __VA_ARGS__)
#define FE_180(WHAT, cls, X, ...) WHAT(cls, X) FE_179(WHAT, cls, __VA_ARGS__)
#define FE_181(WHAT, cls, X, ...) WHAT(cls, X) FE_180(WHAT, cls, __VA_ARGS__)
#define FE_182(WHAT, cls, X, ...) WHAT(cls, X) FE_181(WHAT, cls, __VA_ARGS__)
#define FE_183(WHAT, cls, X, ...) WHAT(cls, X) FE_182(WHAT, cls, __VA_ARGS__)
#define FE_184(WHAT, cls, X, ...) WHAT(cls, X) FE_183(WHAT, cls, __VA_ARGS__)
#define FE_185(WHAT, cls, X, ...) WHAT(cls, X) FE_184(WHAT, cls, __VA_ARGS__)
#define FE_186(WHAT, cls, X, ...) WHAT(cls, X) FE_185(WHAT, cls, __VA_ARGS__)
#define FE_187(WHAT, cls, X, ...) WHAT(cls, X) FE_186(WHAT, cls, __VA_ARGS__)
#define FE_188(WHAT, cls, X, ...) WHAT(cls, X) FE_187(WHAT, cls, __VA_ARGS__)
#define FE_189(WHAT, cls, X, ...) WHAT(cls, X) FE_188(WHAT, cls, __VA_ARGS__)
#define FE_190(WHAT, cls, X, ...) WHAT(cls, X) FE_189(WHAT, cls, __VA_ARGS__)
#define FE_191(WHAT, cls, X, ...) WHAT(cls, X) FE_190(WHAT, cls, __VA_ARGS__)
#define FE_192(WHAT, cls, X, ...) WHAT(cls, X) FE_191(WHAT, cls, __VA_ARGS__)
#define FE_193(WHAT, cls, X, ...) WHAT(cls, X) FE_192(WHAT, cls, __VA_ARGS__)
#define FE_194(WHAT, cls, X, ...) WHAT(cls, X) FE_193(WHAT, cls, __VA_ARGS__)
#define FE_195(WHAT, cls, X, ...) WHAT(cls, X) FE_194(WHAT, cls, __VA_ARGS__)
#define FE_196(WHAT, cls, X, ...) WHAT(cls, X) FE_195(WHAT, cls, __VA_ARGS__)
#define FE_197(WHAT, cls, X, ...) WHAT(cls, X) FE_196(WHAT, cls, __VA_ARGS__)
#define FE_198(WHAT, cls, X, ...) WHAT(cls, X) FE_197(WHAT, cls, __VA_ARGS__)
#define FE_199(WHAT, cls, X, ...) WHAT(cls, X) FE_198(WHAT, cls, __VA_ARGS__)

#define GET_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
    _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26,\
    _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,\
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54,\
    _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68,\
    _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82,\
    _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, \
    _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, \
    _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, \
    _122, _123, _124, _125, _126, _127, _128, _129, _130, _131, _132, _133, \
    _134, _135, _136, _137, _138, _139, _140, _141, _142, _143, _144, _145, \
    _146, _147, _148, _149, _150, _151, _152, _153, _154, _155, _156, _157, \
    _158, _159, _160, _161, _162, _163, _164, _165, _166, _167, _168, _169, \
    _170, _171, _172, _173, _174, _175, _176, _177, _178, _179, _180, _181, \
    _182, _183, _184, _185, _186, _187, _188, _189, _190, _191, _192, _193, \
    _194, _195, _196, _197, _198, _199, NAME, ...) NAME \

#define FOR_EACH(action, cls, ...) \
  GET_MACRO(_0, __VA_ARGS__,        \
        FE_199, FE_198, FE_197, FE_196, FE_195, FE_194, FE_193, FE_192, FE_191, FE_190, \
        FE_189, FE_188, FE_187, FE_186, FE_185, FE_184, FE_183, FE_182, FE_181, FE_180, \
        FE_179, FE_178, FE_177, FE_176, FE_175, FE_174, FE_173, FE_172, FE_171, FE_170, \
        FE_169, FE_168, FE_167, FE_166, FE_165, FE_164, FE_163, FE_162, FE_161, FE_160, \
        FE_159, FE_158, FE_157, FE_156, FE_155, FE_154, FE_153, FE_152, FE_151, FE_150, \
        FE_149, FE_148, FE_147, FE_146, FE_145, FE_144, FE_143, FE_142, FE_141, FE_140, \
        FE_139, FE_138, FE_137, FE_136, FE_135, FE_134, FE_133, FE_132, FE_131, FE_130, \
        FE_129, FE_128, FE_127, FE_126, FE_125, FE_124, FE_123, FE_122, FE_121, FE_120, \
        FE_119, FE_118, FE_117, FE_116, FE_115, FE_114, FE_113, FE_112, FE_111, FE_110, \
        FE_109, FE_108, FE_107, FE_106, FE_105, FE_104, FE_103, FE_102, FE_101, FE_100, \
        FE_99, FE_98, FE_97, FE_96, FE_95, FE_94, FE_93, FE_92, FE_91, FE_90, \
        FE_89, FE_88, FE_87, FE_86, FE_85, FE_84, FE_83, FE_82, FE_81, FE_80, \
        FE_79, FE_78, FE_77, FE_76, FE_75, FE_74, FE_73, FE_72, FE_71, FE_70, \
        FE_69, FE_68, FE_67, FE_66, FE_65, FE_64, FE_63, FE_62, FE_61, FE_60, \
        FE_59, FE_58, FE_57, FE_56, FE_55, FE_54, FE_53, FE_52, FE_51, FE_50, \
        FE_49, FE_48, FE_47, FE_46, FE_45, FE_44, FE_43, FE_42, FE_41, FE_40, \
        FE_39, FE_38, FE_37, FE_36, FE_35, FE_34, FE_33, FE_32, FE_31, FE_30, \
        FE_29, FE_28, FE_27, FE_26, FE_25, FE_24, FE_23, FE_22, FE_21, FE_20, \
        FE_19, FE_18, FE_17, FE_16, FE_15, FE_14, FE_13, FE_12, FE_11, FE_10, \
        FE_9, FE_8, FE_7, FE_6, FE_5, FE_4, FE_3, FE_2, FE_1, FE_0)(action, cls, __VA_ARGS__)

namespace realm {
    template<typename T, typename = void>
    struct managed;
}

#define DECLARE_PERSISTED(cls, property) managed<decltype(cls::property)> property;
#define DECLARE_PROPERTY(cls, p) realm::property<&cls::p>(#p),
#define DECLARE_MANAGED_PROPERTY(cls, p) &realm::managed<cls>::p,
#define DECLARE_MANAGED_PROPERTY_NAME(cls, p) #p,
#define DECLARE_COND_UNMANAGED_TO_MANAGED(cls, p) if constexpr (std::is_same_v<decltype(ptr), decltype(&cls::p)>) { return &managed<cls>::p; }

#include <utility>

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm {
    class rbool;

    struct managed_base {
    protected:
        managed_base() = default;
        managed_base(const managed_base& other) {
            m_obj = other.m_obj;
            m_realm = other.m_realm;
            m_key = other.m_key;
            m_rbool_query = other.m_rbool_query;
        }
        managed_base& operator=(const managed_base& other) {
            m_obj = other.m_obj;
            m_realm = other.m_realm;
            m_key = other.m_key;
            m_rbool_query = other.m_rbool_query;
            return *this;
        }
        managed_base(managed_base&& other) {
            m_obj = std::move(other.m_obj);
            m_realm = std::move(other.m_realm);
            m_key = std::move(other.m_key);
            m_rbool_query = std::move(other.m_rbool_query);
        }
        managed_base& operator=(managed_base&& other) {
            m_obj = std::move(other.m_obj);
            m_realm = std::move(other.m_realm);
            m_key = std::move(other.m_key);
            m_rbool_query = std::move(other.m_rbool_query);
            return *this;
        }
        ~managed_base() {
            m_obj = nullptr;
            m_realm = nullptr;
            m_rbool_query = nullptr;
            m_key.~col_key();
        }
    public:
        static constexpr bool is_object = false;
        internal::bridge::obj *m_obj = nullptr;
        internal::bridge::realm *m_realm = nullptr;
        internal::bridge::col_key m_key;
        // MARK: Queries
        rbool* m_rbool_query = nullptr;

        void assign(internal::bridge::obj *obj,
                    internal::bridge::realm* realm,
                    internal::bridge::col_key &&key) {
            this->m_obj = obj;
            this->m_realm = realm;
            this->m_key = key;
        }

        void assign(internal::bridge::obj *obj,
                    internal::bridge::realm* realm,
                    const internal::bridge::col_key &key) {
            this->m_obj = obj;
            this->m_realm = realm;
            this->m_key = key;
        }

        template<typename T>
        managed_base& operator =(const T& v) {
            if constexpr (std::is_same_v<T, std::nullopt_t>) {
                m_obj->set_null(m_key);
            } else {
                m_obj->template set<typename internal::type_info::type_info<T, void>::internal_type>(m_key, v);
            }
            return *this;
        }

        void prepare_for_query(internal::bridge::realm* realm,
                               const internal::bridge::table& table,
                               const std::string_view& col_name,
                               realm::rbool* query_builder) {
            this->m_realm = realm;
            this->m_key = table.get_column_key(col_name);
            this->m_rbool_query = query_builder;
        }
    };

    template<typename T, typename>
    struct managed;
}

#define __cpprealm_build_query(op, name, type) \
rbool managed<type>::operator op(const type& rhs) const noexcept { \
    if (this->m_rbool_query) { \
        return this->m_rbool_query->name(m_key, rhs); \
    } \
    return serialize(detach()) op serialize(rhs); \
} \

#define __cpprealm_build_optional_query(op, name, type) \
rbool managed<std::optional<type>>::operator op(const std::optional<type>& rhs) const noexcept { \
    if (this->m_rbool_query) {        \
        return this->m_rbool_query->name(m_key, rhs); \
    } \
    return serialize(detach()) op serialize(rhs); \
} \

#define REALM_SCHEMA(cls, ...) \
    DECLARE_REALM_SCHEMA(cls, false, false, ObjectType::TopLevel, __VA_ARGS__) \

#define REALM_EMBEDDED_SCHEMA(cls, ...) \
    DECLARE_REALM_SCHEMA(cls, true, false, ObjectType::Embedded, __VA_ARGS__)

#define REALM_ASYMMETRIC_SCHEMA(cls, ...) \
    DECLARE_REALM_SCHEMA(cls, false, true, ObjectType::Asymmetric, __VA_ARGS__)

#define DECLARE_REALM_SCHEMA(cls, is_embedded_object, is_asymmetric_object, obj_type, ...) \
    template <> struct managed<cls> { \
        managed() = default; \
        static constexpr ObjectType object_type = obj_type;     \
        FOR_EACH(DECLARE_PERSISTED, cls, __VA_ARGS__) \
        static constexpr auto schema = realm::schema(#cls, object_type, std::tuple{ FOR_EACH(DECLARE_PROPERTY, cls, __VA_ARGS__) }  ); \
        static constexpr auto managed_pointers() { \
            return std::tuple{FOR_EACH(DECLARE_MANAGED_PROPERTY, cls, __VA_ARGS__)};  \
        }                                                                                          \
        template <typename PtrType> static constexpr auto unmanaged_to_managed_pointer(PtrType ptr) {           \
           FOR_EACH(DECLARE_COND_UNMANAGED_TO_MANAGED, cls, __VA_ARGS__);                                       \
        }                                                                                                       \
        static constexpr auto managed_pointers_names() {                                                        \
            constexpr auto managed_pointers_names = std::array<std::string_view, std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>{FOR_EACH(DECLARE_MANAGED_PROPERTY_NAME, cls, __VA_ARGS__)}; \
            return internal::array_to_tuple(managed_pointers_names);                                            \
        }                                                                                                       \
                                                                                                                \
        static constexpr bool is_object = true;                                                                 \
        explicit managed(const internal::bridge::obj& obj,                                                      \
                         internal::bridge::realm realm)                                                         \
        : m_obj(std::move(obj))                                                                                 \
        , m_realm(std::move(realm))                                                                             \
        {                                                                                                       \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            std::apply([&](auto& ...pair) {                                                                     \
                ((*this.*pair.first).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(pair.second)), ...); \
            }, zipped);                                                                                         \
        }                                                                                                       \
        managed(const managed& other) {                                                                         \
            m_obj = other.m_obj;                                                                                \
            m_realm = other.m_realm;                                                                            \
            m_rbool_query = other.m_rbool_query;                                                                \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            if (m_rbool_query) {                                                                                \
                auto schema = m_realm.schema().find(other.schema.name);                                         \
                auto group = m_realm.read_group();                                                              \
                auto table_ref = group.get_table(schema.table_key());                                           \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).prepare_for_query(&m_realm, table_ref, pair.second, m_rbool_query), ...);  \
                }, zipped);                                                                                     \
            } else {                                                                                            \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(pair.second)), ...); \
                }, zipped);                                                                                     \
            }                                                                                                   \
        }                                                                                                       \
        managed& operator=(const managed& other) {                                                              \
            m_obj = other.m_obj;                                                                                \
            m_realm = other.m_realm;                                                                            \
            m_rbool_query = other.m_rbool_query;                                                                \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            if (m_rbool_query) {                                                                                \
                auto schema = m_realm.schema().find(other.schema.name);                                         \
                auto group = m_realm.read_group();                                                              \
                auto table_ref = group.get_table(schema.table_key());                                           \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).prepare_for_query(&m_realm, table_ref, pair.second, m_rbool_query), ...);  \
                }, zipped);                                                                                     \
            } else {                                                                                            \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(pair.second)), ...); \
                }, zipped);                                                                                     \
            }                                                                                                   \
            return *this;                                                                                       \
        }                                                                                                       \
        managed(managed&& other) {                                                                              \
            m_obj = std::move(other.m_obj);                                                                     \
            m_realm = std::move(other.m_realm);                                                                 \
            m_rbool_query = std::move(other.m_rbool_query);                                                     \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            if (m_rbool_query) {                                                                                \
                auto schema = m_realm.schema().find(other.schema.name);                                         \
                auto group = m_realm.read_group();                                                              \
                auto table_ref = group.get_table(schema.table_key());                                           \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).prepare_for_query(&m_realm, table_ref, pair.second, m_rbool_query), ...);  \
                }, zipped);                                                                                     \
             } else {                                                                                           \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(pair.second)), ...); \
                }, zipped);                                                                                     \
             }                                                                                                  \
        }                                                                                                       \
        managed& operator=(managed&& other) {                                                                   \
            m_obj = std::move(other.m_obj);                                                                     \
            m_realm = std::move(other.m_realm);                                                                 \
            m_rbool_query = std::move(other.m_rbool_query);                                                     \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            if (m_rbool_query) {                                                                                \
                auto schema = m_realm.schema().find(other.schema.name);                                         \
                auto group = m_realm.read_group();                                                              \
                auto table_ref = group.get_table(schema.table_key());                                           \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).prepare_for_query(&m_realm, table_ref, pair.second, m_rbool_query), ...);  \
                }, zipped);                                                                                     \
            } else {                                                                                            \
                std::apply([&](auto& ...pair) {                                                                 \
                    ((*this.*pair.first).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(pair.second)), ...); \
                }, zipped);                                                                                     \
            }                                                                                                   \
            return *this;                                                                                       \
        }                                                                                                       \
        static managed prepare_for_query(const internal::bridge::realm& r, realm::rbool* q) {                   \
            managed<cls> m;                                                                                     \
            m.m_realm = r;                                                                                      \
            m.m_rbool_query = q;                                                                                \
            auto schema = m.m_realm.schema().find(m.schema.name);                                               \
            auto group = m.m_realm.read_group();                                                                \
            auto table_ref = group.get_table(schema.table_key());                                               \
            auto zipped = internal::zip_tuples(managed_pointers(), managed_pointers_names());                   \
            std::apply([&m, &table_ref](auto& ...pair) {                                                        \
                ((m.*pair.first).prepare_for_query(&m.m_realm, table_ref, pair.second, m.m_rbool_query), ...);  \
            }, zipped);                                                                             \
            return m;                                                                               \
        }                                                                                           \
        cls detach() const {                                                                        \
            cls v;                                                                                  \
            auto assign = [&v, this](auto& pair) {                                                  \
                v.*(std::decay_t<decltype(pair.first)>::ptr) = ((*this).*(pair.second)).detach();   \
            };                                                                                      \
            auto zipped = internal::zip_tuples(managed<cls>::schema.ps, managed<cls>::managed_pointers()); \
            std::apply([&assign](auto& ...pair) {                                                   \
                (assign(pair), ...);                                                                \
            }, zipped);                                                                             \
            return v;                                                                               \
        }                                                                                           \
        auto observe(std::function<void(realm::object_change<managed>&&)>&& fn) {                   \
            auto m_object = std::make_shared<internal::bridge::object>(m_realm, m_obj);             \
            auto wrapper = realm::object_change_callback_wrapper<managed>{                          \
            std::move(fn), this, m_object};                                                         \
            return m_object->add_notification_callback(                                             \
            std::make_shared<realm::object_change_callback_wrapper<managed>>(wrapper));             \
        }                                                                                           \
        bool is_invalidated() {                                                                     \
            return !m_obj.is_valid();                                                               \
        }                                                                                           \
        bool is_frozen() {                                                                          \
            return m_realm.is_frozen();                                                             \
        }                                                                                           \
        managed<cls> freeze() {                                                                     \
            auto realm = m_realm.freeze();                                                          \
            return managed<cls>(realm.import_copy_of(m_obj), realm);                                \
        }                                                                                           \
        managed<cls> thaw() {                                                                       \
            if (is_invalidated()) {                                                                 \
                throw std::runtime_error("Invalid objects cannot be thawed.");                      \
            }                                                                                       \
            if (!is_frozen()) {                                                                     \
                return *this;                                                                       \
            }                                                                                       \
            auto thawed_realm = m_realm.thaw();                                                     \
            return managed<cls>(thawed_realm.import_copy_of(m_obj), thawed_realm);                  \
        }                                                                                           \
        db get_realm() {                                                                            \
            return db(m_realm);                                                                     \
        }                                                                                           \
        bool operator ==(const managed<cls>& other) const {                                        \
            if (m_rbool_query != nullptr)                                                           \
                throw std::runtime_error("This comparison operator is not valid inside of `where`");    \
            auto& a = m_obj;                                                                        \
            auto& b = other.m_obj;                                                                  \
            if (m_realm != other.m_realm) {                                                         \
                return false;                                                                       \
            }                                                                                       \
            return a.get_table() == b.get_table()                                                   \
                   && a.get_key() == b.get_key();                                                   \
        }                                                                                           \
        bool operator ==(const managed<cls*>& other) const {                                        \
            if (m_rbool_query != nullptr)                                                           \
                throw std::runtime_error("This comparison operator is not valid inside of `where`");    \
            auto& a = m_obj;                                                                        \
            auto& b = other.m_obj;                                                                  \
            if (m_realm != other->m_realm) {                                                        \
                return false;                                                                       \
            }                                                                                       \
            return a.get_table() == b->get_table()                                                  \
                   && a.get_key() == b->get_key();                                                  \
        }                                                                                           \
        bool operator !=(const managed<cls>& other) const {                                         \
           return !this->operator ==(other);                                                        \
        }                                                                                           \
        bool operator !=(const managed<cls*>& other) const {                                        \
            return !this->operator ==(other);                                                       \
        }                                                                                           \
        bool operator < (const managed<cls>& rhs) const {                                           \
            return m_obj.get_key() < rhs.m_obj.get_key();                                           \
        }                                                                                           \
        void to_json(std::ostream& out) const noexcept {                                            \
            m_obj.to_json(out);                                                                     \
        }                                                                                           \
    private:                                                                                        \
        internal::bridge::obj m_obj;                                                                \
        internal::bridge::realm m_realm;                                                            \
        rbool* m_rbool_query = nullptr;                                                             \
        friend struct db;                                                                           \
        template <typename, typename> friend struct managed;                                        \
        template <typename, typename> friend struct box;                                            \
        template <typename> friend struct ::realm::thread_safe_reference;                           \
        template <typename T> friend rbool* ::realm::internal::get_rbool(const T&);                 \
                                                                                                    \
    };                                                                                              \
    struct meta_schema_##cls {                                                                      \
        meta_schema_##cls() {                                                                       \
            auto s = managed<cls>::schema.to_core_schema();                                         \
            auto it = std::find(std::begin(realm::db::schemas), std::end(realm::db::schemas), s);   \
            if (it == std::end(realm::db::schemas))                                                 \
                realm::db::schemas.push_back(s);                                                    \
        }                                                                                           \
    };                                                                                              \
    static inline meta_schema_##cls _meta_schema_##cls{};

namespace realm::internal {
    /*
     * Helper method for extracting the private `m_rbool_query`
     * property on a managed<T> object.
     */
    template<typename T>
    rbool* get_rbool(const T& o) {
        return o.m_rbool_query;
    }
}

#endif //CPPREALM_MACROS_HPP
