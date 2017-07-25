using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SharpSvn.TestBuilder;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;

namespace SharpSvn.Tests.LookCommands
{
    [TestClass]
    public class InheritedPropertyTests : HookTestBase
    {
        [TestMethod]
        public void LookInheritedProps()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri uri = sbox.CreateRepository(SandBoxRepository.Greek);

            using (InstallHook(uri, SvnHookType.PreCommit, OnChangedProps))
            {
                Client.RepositoryOperation(new Uri(uri, "trunk"), delegate(SvnMultiCommandClient cl)
                    {
                        cl.SetProperty("", "root-A", "B");
                        cl.SetProperty("A", "A-A", "C");
                        cl.SetProperty("A/mu", "mu-A", "D");
                    });
            }

            using (SvnLookClient look = new SvnLookClient())
            {
                int i = 0;
                look.InheritedPropertyList(new SvnLookOrigin(uri.LocalPath, 2), "trunk/A/mu",
                    delegate(object sender2, SvnLookInheritedPropertyListEventArgs ee)
                    {
                        switch (i++)
                        {
                            case 0:
                                Assert.That(ee.Path, Is.EqualTo("trunk"));
                                Assert.That(ee.Properties.Contains("root-A"));
                                break;
                            case 1:
                                Assert.That(ee.Path, Is.EqualTo("trunk/A"));
                                Assert.That(ee.Properties.Contains("A-A"));
                                break;
                            case 2:
                                Assert.That(ee.Path, Is.EqualTo("trunk/A/mu"));
                                Assert.That(ee.Properties.Contains("mu-A"));
                                break;
                            default:
                                throw new InvalidOperationException("Too many invocations");
                        }
                    });

                Assert.That(i, Is.GreaterThan(0), "No props listed");
            }

        }

        private void OnChangedProps(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient look = new SvnLookClient())
            {
                int i = 0;
                look.InheritedPropertyList(e.HookArgs.LookOrigin, "trunk/A/mu",
                    delegate(object sender2, SvnLookInheritedPropertyListEventArgs ee)
                    {
                        switch (i++)
                        {
                            case 0:
                                Assert.That(ee.Path, Is.EqualTo("trunk"));
                                Assert.That(ee.Properties.Contains("root-A"));
                                break;
                            case 1:
                                Assert.That(ee.Path, Is.EqualTo("trunk/A"));
                                Assert.That(ee.Properties.Contains("A-A"));
                                break;
                            case 2:
                                Assert.That(ee.Path, Is.EqualTo("trunk/A/mu"));
                                Assert.That(ee.Properties.Contains("mu-A"));
                                break;
                            default:
                                throw new InvalidOperationException("Too many invocations");
                        }
                    });

                Assert.That(i, Is.GreaterThan(0), "No props listed");
            }
        }

    }
}
