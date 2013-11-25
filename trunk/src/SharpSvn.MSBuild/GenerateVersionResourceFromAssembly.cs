using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using Microsoft.Build.Framework;

namespace SharpSvn.MSBuild
{
	public class GenerateVersionResourceFromAssembly : ITask
    {
		public Microsoft.Build.Framework.IBuildEngine BuildEngine
		{
			get; set;
		}

		[Required]
		public ITaskItem Source
		{
			get; set;
		}

		[Required]
		public ITaskItem OutputFile
		{
			get; set;
		}

		[Required]
		public string TempDir
		{
			get; set;
		}

		public Microsoft.Build.Framework.ITaskHost HostObject
		{
			get; set;
		}

		public bool Execute()
		{
			Assembly myAssembly = typeof(GenerateVersionResourceFromAssembly).Assembly;
			AppDomainSetup setup = new AppDomainSetup();
			setup.ApplicationName = "GenerateVersionRcFromAssembly";
			setup.ApplicationBase = Path.GetDirectoryName(new Uri(myAssembly.CodeBase).LocalPath);
			setup.AppDomainInitializer = new AppDomainInitializer(OnRefreshVersionInfo);
			setup.AppDomainInitializerArguments = new string[] { Source.ItemSpec, TempDir, OutputFile.ItemSpec };

			AppDomain dom = AppDomain.CreateDomain("AttributeRefresher", myAssembly.Evidence, setup);
			AppDomain.Unload(dom); // Remove locks
			return true;
		}

		// Called in it's own appdomain
		static void OnRefreshVersionInfo(string[] args)
		{
			string fromFile = args[0];
			string toDir = args[1];
			string toFile = args[2];

			Assembly asm = Assembly.ReflectionOnlyLoad(File.ReadAllBytes(fromFile));

			string result = GenerateAttributeAssembly(asm, toDir);
		}

		private static string GenerateAttributeAssembly(Assembly assembly, string outputDirectory)
		{
			AssemblyName srcName = new AssemblyName(assembly.FullName);

			if (srcName == null || string.IsNullOrEmpty(srcName.Name))
				return null;

			try
			{
				// Prepare dynamic assembly for resources
				AssemblyName asmName = new AssemblyName(srcName.FullName);
				asmName.Name = "Tmp." + srcName.Name;

				// Only create an on-disk assembly. We never have to execute anything
				AssemblyBuilder newAssembly = AppDomain.CurrentDomain.DefineDynamicAssembly(asmName, AssemblyBuilderAccess.ReflectionOnly, outputDirectory);

				string tmpFile = srcName.Name + ".dll";
				newAssembly.DefineDynamicModule(asmName.Name, tmpFile);

				AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve += new ResolveEventHandler(OnReflectionOnlyAssemblyResolve);

				try
				{
					Assembly mscorlib = Assembly.ReflectionOnlyLoad(typeof(int).Assembly.FullName);
					Assembly system = Assembly.ReflectionOnlyLoad(typeof(Uri).Assembly.FullName);
					bool hasInformationalVersion = false;
					bool hasVersion = false;

					foreach (CustomAttributeData attr in CustomAttributeData.GetCustomAttributes(assembly))
					{
						if ((attr.NamedArguments.Count > 0) || (attr.Constructor == null))
						{
							// We don't use named arguments at this time; not needed for the version resources
							continue;
						}

						Type type = attr.Constructor.ReflectedType;

						if (type.Assembly != mscorlib && type.Assembly != system)
						{
							continue;
						}

						if (type.Assembly == mscorlib)
							switch (type.Name)
							{
								case "System.Reflection.AssemblyInformationalVersionAttribute":
									hasInformationalVersion = true;
									break;
								case "System.Reflection.AssemblyVersionAttribute":
									hasVersion = true;
									break;
							}

						Console.Error.WriteLine("Adding: {0}", type.FullName);

						List<object> values = new List<object>();
						foreach (CustomAttributeTypedArgument arg in attr.ConstructorArguments)
						{
							values.Add(arg.Value);
						}

						CustomAttributeBuilder cb = new CustomAttributeBuilder(attr.Constructor, values.ToArray());

						newAssembly.SetCustomAttribute(cb);
					}

					Console.Error.WriteLine("Extra: {0}, {1}", hasVersion, hasInformationalVersion);

					if (!hasVersion)
						newAssembly.SetCustomAttribute(
								new CustomAttributeBuilder(typeof(AssemblyVersionAttribute).GetConstructor(new Type[] { typeof(String) }),
														   new object[] { srcName.Version.ToString() }));
					if (!hasInformationalVersion)
						newAssembly.SetCustomAttribute(
								new CustomAttributeBuilder(typeof(AssemblyInformationalVersionAttribute).GetConstructor(new Type[] { typeof(String) }),
														   new object[] { srcName.Version.ToString() }));

					newAssembly.SetCustomAttribute(
								new CustomAttributeBuilder(typeof(AssemblyCultureAttribute).GetConstructor(new Type[] { typeof(String) }),
															new object[] { "" }));
				}
				finally
				{
					AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve -= new ResolveEventHandler(OnReflectionOnlyAssemblyResolve);
				}

				newAssembly.DefineVersionInfoResource();
				Console.Error.WriteLine("Writing to {0}", tmpFile);
				newAssembly.Save(tmpFile);

				return Path.Combine(outputDirectory, tmpFile);
			}
			catch (FileLoadException)
			{
				return null;
			}
			catch (IOException)
			{
				return null;
			}
		}

		/// <summary>
		/// Called when [reflection only assembly resolve].
		/// </summary>
		/// <param name="sender">The sender.</param>
		/// <param name="args">The <see cref="System.ResolveEventArgs"/> instance containing the event data.</param>
		/// <returns></returns>
		static Assembly OnReflectionOnlyAssemblyResolve(object sender, ResolveEventArgs args)
		{
			return Assembly.ReflectionOnlyLoad(args.Name);
		}
	}
}
