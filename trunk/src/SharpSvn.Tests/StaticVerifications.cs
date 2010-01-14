using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.Reflection;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class StaticVerifications
    {
        [Test]
        public void VerifyEnums()
        {
            Assembly asm = typeof(SvnClient).Assembly;

            foreach (Type tp in asm.GetTypes())
            {
                if (!tp.IsEnum || !tp.IsPublic)
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
                else if (tp == typeof(SvnChangeAction))
                    continue;
                else if (tp == typeof(SvnErrorCode))
                    continue;

                for (int i = min; i <= max; i++)
                {
                    Assert.That(Enum.IsDefined(tp, i), "{0} value is defined in {1}", i, tp);
                }
            }
        }
    }
}
