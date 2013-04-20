using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using NUnit.Framework;
using SharpGit.Plumbing;

namespace SharpGit.Tests
{
    [TestFixture]
    public class GitClientTests
    {
        string GetTempPath()
        {
            return Path.Combine(Path.GetTempPath(), "gittest\\"+Guid.NewGuid().ToString("N"));
        }

        [Test]
        public void MinVersion()
        {
            Assert.That(GitClient.Version, Is.GreaterThanOrEqualTo(new Version(0, 17)));

            Assert.That(GitClient.SharpGitVersion, Is.GreaterThanOrEqualTo(new Version(0, 1700)));
            Assert.That(GitClient.AdministrativeDirectoryName, Is.EqualTo(".git"));
            Assert.That(GitClient.GitLibraries, Is.Not.Empty);
        }

        [Test]
        public void UseGitClient()
        {
            string dir = GetTempPath();
            using (GitRepository repo = GitRepository.Create(dir))
            using (GitClient git = new GitClient())
            {
                string ignoreFile = Path.Combine(dir, ".gitignore");
                string file = Path.Combine(dir, "newfile");
                string subDir = Path.Combine(dir, "dir");
                string fileInSubDir = Path.Combine(subDir, "file2");
                string file3 = Path.Combine(dir, "other");
                string file4 = Path.Combine(dir, "q.ignore");
                File.WriteAllText(file, "Some body");
                Directory.CreateDirectory(subDir);
                File.WriteAllText(fileInSubDir, "Some other body");
                File.WriteAllText(file3, "file3");

                File.WriteAllText(ignoreFile, "*.ignore\n");
                File.WriteAllText(file4, "file4");

                git.Add(ignoreFile);
                git.Add(file);
                git.Add(fileInSubDir);

                int ticked = 0;

                GitStatusArgs gsa = new GitStatusArgs();
                gsa.IncludeIgnored = true;

                Assert.That(git.Status(dir, gsa,
                    delegate(object sender, GitStatusEventArgs e)
                    {
                        switch (e.RelativePath)
                        {
                            case "newfile":
                                //Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added));
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case "dir/file2":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case "other":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.New));
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case ".gitignore":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case "q.ignore":
                                // TODO: Make this ignored
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.Ignored, Is.True);
                                break;
                            default:
                                Assert.Fail("Invalid node found: {0}", e.RelativePath);
                                break;
                        }

                        Assert.That(e.FullPath, Is.EqualTo(Path.GetFullPath(Path.Combine(dir, e.RelativePath))));
                        ticked++;
                    }), Is.True);

                Assert.That(ticked, Is.EqualTo(5), "Ticked");

                Assert.That(git.Delete(fileInSubDir));
            }

        }

        [Test]
        public void CreateGitRepository()
        {
            string dir = GetTempPath();
            string file = Path.Combine(dir, "file");
            using (GitRepository repo = GitRepository.Create(dir))
            {
                Assert.That(repo, Is.Not.Null);
                Assert.That(repo.IsEmpty, Is.True);
                Assert.That(repo.IsBare, Is.False);
                Assert.That(repo.RepositoryPath, Is.EqualTo(Path.Combine(dir, GitClient.AdministrativeDirectoryName)));
                Assert.That(repo.WorkingPath, Is.EqualTo(dir));

                GitConfiguration config = repo.Configuration;

                Assert.That(config, Is.Not.Null);
                repo.SetConfiguration(config);

                GitIndex index = repo.Index;
                Assert.That(index, Is.Not.Null);
                repo.SetIndex(index);

                GitObjectDatabase odb = repo.ObjectDatabase;
                Assert.That(odb, Is.Not.Null);
                repo.SetObjectDatabase(odb);

                File.WriteAllText(file, "qqq");

                repo.Index.Add("file");


                Assert.That(index.Contains("file"));
                GitIndexEntry entry = repo.Index["file"];

                Assert.That(entry, Is.Not.Null);
                Assert.That(entry.FileSize, Is.EqualTo(3));
                Assert.That(entry.Id, Is.EqualTo(new GitId("E5A49F32170B89EE4425C4AB09E70DFCDB93E174")));

                index.Reload();

                Assert.That(index.Contains("file"), Is.False);
            }
        }
    }
}
