using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using System.Reflection;
using System.Collections;

namespace SharpSvn.Tests
{
    [TestClass]
    public class StaticVerifications
    {
        [TestMethod]
        public void StaticVerifications_VerifyEnums()
        {
            Assembly asm = typeof(SvnClient).Assembly;
            Hashtable ignoreTypes = new Hashtable();
            ignoreTypes.Add(typeof(SvnErrorCode), asm);
            ignoreTypes.Add(typeof(SvnWindowsErrorCode), asm);
            ignoreTypes.Add(typeof(SvnChangeAction), asm);
            ignoreTypes.Add(typeof(SvnAprErrorCode), asm);

            foreach (Type tp in asm.GetTypes())
            {
                if (!tp.IsEnum || !tp.IsPublic || ignoreTypes.Contains(tp))
                    continue;

                if (tp.GetCustomAttributes(typeof(FlagsAttribute), false).Length == 1 || Enum.GetUnderlyingType(tp) != typeof(int))
                    continue;

                Assert.That(Enum.IsDefined(tp, 0), "0 value is defined in {0}", tp);

                int min = 0;
                int max = 0;

                foreach (int v in Enum.GetValues(tp))
                {
                    if (v < min)
                        min = v;

                    if (v > max)
                        max = v;
                }

                if (tp == typeof(SvnCommandType))
                    max = (int)SvnCommandType.Write;

                for (int i = min; i <= max; i++)
                {
                    Assert.That(Enum.IsDefined(tp, i), "{0} value is defined in {1}", i, tp);
                }
            }
        }
    }
}
