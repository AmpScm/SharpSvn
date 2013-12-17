using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SharpSvn.TestBuilder
{
    public interface IHasTestContext
    {
        TestContext TestContext { get; }
    }
}
