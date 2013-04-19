using NUnit.Framework;
using SharpGit.Plumbing;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpGit.Tests
{
    [TestFixture]
    public class GitClientTests
    {
        string GetTempPath()
        {
            return Path.Combine(Path.GetTempPath(), "gittest-"+Guid.NewGuid().ToString("N"));
        }

        [Test]
        public void MinVersion()
        {
            Assert.That(GitClient.Version, Is.GreaterThanOrEqualTo(new Version(0, 17)));

            Assert.That(GitClient.SharpGitVersion, Is.GreaterThanOrEqualTo(new Version(0, 1700)));
            Assert.That(GitClient.AdministrativeDirectoryName, Is.EqualTo(".git"));
        }

        [Test]
        public void UseGitClient()
        {
            string dir = GetTempPath();
            using (GitRepository repo = GitRepository.Create(dir))
            {
                using (GitClient gc = new GitClient())
                {
                    Assert.That(gc.Status(dir, new GitStatusArgs(), delegate(object sender, GitStatusEventArgs e)
                    {
                    }), Is.False);
                }
            }
        }

        [Test]
        public void CreateGitRepository()
        {
            string dir = GetTempPath();
            using (GitRepository repo = GitRepository.Create(dir))
            {
                Assert.That(repo, Is.Not.Null);
                Assert.That(repo.IsEmpty, Is.True);
                Assert.That(repo.IsBare, Is.False);
                Assert.That(repo.RepositoryPath, Is.EqualTo(Path.Combine(dir, GitClient.AdministrativeDirectoryName)));
                Assert.That(repo.WorkingPath, Is.EqualTo(dir));

                GitConfiguration config = repo.GetConfigurationInstance();

                Assert.That(config, Is.Not.Null);
                repo.SetConfiguration(config);

                GitIndex index = repo.GetIndexInstance();
                Assert.That(index, Is.Not.Null);
                repo.SetIndex(index);

                GitObjectDatabase odb = repo.GetObjectDatabaseInstance();
                Assert.That(odb, Is.Not.Null);
                repo.SetObjectDatabase(odb);
            }
        }
    }
}
