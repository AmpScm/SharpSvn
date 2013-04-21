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
            GitCommitArgs ga = new GitCommitArgs();
            ga.Author.Name = "Tester";
            ga.Author.EmailAddress = "author@example.com";
            ga.Committer.Name = "Other";
            ga.Committer.EmailAddress = "committer@example.com";
            DateTime ct = new DateTime(2002, 01, 01, 0, 0, 0, DateTimeKind.Utc);
            ga.Author.When = ct;
            ga.Committer.When = ct;

            string repoDir = GetTempPath();
            string repo2Dir = GetTempPath();
            using (GitRepository repo = GitRepository.Create(repoDir))
            using (GitClient git = new GitClient())
            {
                string ignoreFile = Path.Combine(repoDir, ".gitignore");
                string file = Path.Combine(repoDir, "newfile");
                string subDir = Path.Combine(repoDir, "dir");
                string fileInSubDir = Path.Combine(subDir, "file2");
                string file3 = Path.Combine(repoDir, "other");
                string file4 = Path.Combine(repoDir, "q.ignore");
                File.WriteAllText(file, "Some body");
                Directory.CreateDirectory(subDir);
                File.WriteAllText(fileInSubDir, "Some other body");
                File.WriteAllText(file3, "file3");

                File.WriteAllText(ignoreFile, "*.ignore\n");
                File.WriteAllText(file4, "file4");

                git.Add(ignoreFile);
                git.Add(file);
                git.Commit(repoDir, ga);

                git.Add(fileInSubDir);

                int ticked = 0;

                File.AppendAllText(file, "\nExtra Line");

                GitStatusArgs gsa = new GitStatusArgs();
                gsa.IncludeIgnored = true;
                gsa.IncludeUnmodified = true;

                Assert.That(git.Status(repoDir, gsa,
                    delegate(object sender, GitStatusEventArgs e)
                    {
                        switch (e.RelativePath)
                        {
                            case "newfile":
                                //Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added));
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal), "newfile index normal");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Modified), "newfile wc modified");
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case "dir/file2":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Added), "file2 index added");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Normal), "file2 wc normal");
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case "other":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal));
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.New));
                                Assert.That(e.Ignored, Is.False);
                                break;
                            case ".gitignore":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal));
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

                        Assert.That(e.FullPath, Is.EqualTo(Path.GetFullPath(Path.Combine(repoDir, e.RelativePath))));
                        ticked++;
                    }), Is.True);

                Assert.That(ticked, Is.EqualTo(5), "Ticked");

                Assert.That(git.Delete(fileInSubDir));

                GitId commit;

                ga.LogMessage = "A log message to remember";

                // The passed path is currently just used to find the local repository
                Assert.That(git.Commit(repoDir, ga, out commit));
                Assert.That(commit, Is.EqualTo(new GitId("fb55a493fe14a38875ceb0ecec50f63025503a79")));

                GitCloneArgs gc = new GitCloneArgs();
                gc.Synchronous = true;

                git.Clone(repoDir, repo2Dir, gc);

            }

            using (GitRepository repo1 = new GitRepository(repoDir))
            //using (GitRepository repo2 = new GitRepository(repo2Dir))
            {
                Assert.That(repo1.Head, Is.Not.Null);
                //Assert.That(repo2.Head, Is.Not.Null);
                
                GitId headId;
                Assert.That(repo1.ResolveReference(repo1.Head, out headId));
                Assert.That(headId, Is.EqualTo(new GitId("fb55a493fe14a38875ceb0ecec50f63025503a79")));
                GitCommit commit;

                Assert.That(repo1.GetCommit(headId, out commit));
                Assert.That(commit, Is.Not.Null, "Have a commit");

                Assert.That(commit.Author, Is.Not.Null);
                Assert.That(commit.Author.Name, Is.EqualTo("Tester"));
                Assert.That(commit.Author.EmailAddress, Is.EqualTo("author@example.com"));

                Assert.That(commit.Committer, Is.Not.Null);
                Assert.That(commit.Committer.Name, Is.EqualTo("Other"));
                Assert.That(commit.Committer.EmailAddress, Is.EqualTo("committer@example.com"));

                // Assert.That(commit.Committer.TimeOffsetInMinutes, Is.EqualTo(120)); // CEST dependent
                Assert.That(commit.LogMessage, Is.EqualTo("A log message to remember"));
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
