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

            // Use stable time and offset to always produce the same hash
            DateTime ct = new DateTime(2002, 01, 01, 0, 0, 0, DateTimeKind.Utc);
            ga.Author.When = ct;
            ga.Author.TimeOffsetInMinutes = 120;
            ga.Committer.When = ct;
            ga.Committer.TimeOffsetInMinutes = 120;

            string repoDir = GetTempPath();
            string repo2Dir = GetTempPath();
            GitId firstResult;
            GitId lastCommit;
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
                git.Commit(repoDir, ga, out firstResult);

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

                ga.LogMessage = "Intermediate";
                git.Commit(repoDir, ga);

                Assert.That(git.Delete(fileInSubDir));
                Assert.That(git.Add(file));

                GitId commit;

                ga.LogMessage = "A log message to remember";

                // The passed path is currently just used to find the local repository
                lastCommit = new GitId("c860e9b866737faae5a047d4dbd07a082b5c0bc9");
                Assert.That(git.Commit(repoDir, ga, out commit));
                Assert.That(commit, Is.EqualTo(lastCommit));

                File.Move(file, file + ".a");

                ticked = 0;
                gsa.IncludeIgnored = false;
                gsa.IncludeUnversioned = true;
                gsa.IncludeUnmodified = false;
                Assert.That(git.Status(repoDir, gsa,
                    delegate(object sender, GitStatusEventArgs e)
                    {
                        switch (e.RelativePath)
                        {
                            case "dir":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal), "dir index normal");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.New), "dir wc normal");
                                break;
                            case "newfile":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal), "newfile index normal");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.Deleted), "newfile wc deleted");
                                break;
                            case "newfile.a":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal), "newfile.a index normal");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.New), "newfile.a wc new");
                                break;
                            case "other":
                                Assert.That(e.IndexStatus, Is.EqualTo(GitStatus.Normal), "other index normal");
                                Assert.That(e.WorkingDirectoryStatus, Is.EqualTo(GitStatus.New), "other wc normal");
                                break;
                            default:
                                Assert.Fail("Invalid node found: {0}", e.RelativePath);
                                break;
                        }

                        Assert.That(e.FullPath, Is.EqualTo(Path.GetFullPath(Path.Combine(repoDir, e.RelativePath))));
                        ticked++;
                    }), Is.True);

                Assert.That(ticked, Is.EqualTo(4));

                GitCloneArgs gc = new GitCloneArgs();
                gc.Synchronous = true;

                git.Clone(repoDir, repo2Dir, gc);
            }

            using (GitRepository repo1 = new GitRepository(repoDir))
            using (GitRepository repo2 = new GitRepository(repo2Dir))
            {
                GitReference head = repo1.Head;
                Assert.That(head, Is.Not.Null, "Has head");

                Assert.That(head.Name, Is.EqualTo("refs/heads/master"));
                //Assert.That(repo2.Head, Is.Not.Null);
                
                GitId headId;
                Assert.That(repo1.ResolveReference(repo1.Head, out headId));
                Assert.That(headId, Is.EqualTo(lastCommit));
                GitCommit commit;

                Assert.That(repo1.GetCommit(headId, out commit));
                Assert.That(commit, Is.Not.Null, "Have a commit");

                Assert.That(commit.Id, Is.EqualTo(lastCommit));
                Assert.That(commit.Ancestors, Is.Not.Empty);
                Assert.That(commit.Ancestor, Is.Not.Null);
                Assert.That(commit.Ancestor.Ancestor, Is.Not.Null);
                Assert.That(commit.Ancestor.Ancestor.Ancestor, Is.Null);
                Assert.That(commit.Ancestor.Ancestor.Id, Is.EqualTo(firstResult));

                Assert.That(commit.Author, Is.Not.Null);
                Assert.That(commit.Author.Name, Is.EqualTo("Tester"));
                Assert.That(commit.Author.EmailAddress, Is.EqualTo("author@example.com"));

                Assert.That(commit.Committer, Is.Not.Null);
                Assert.That(commit.Committer.Name, Is.EqualTo("Other"));
                Assert.That(commit.Committer.EmailAddress, Is.EqualTo("committer@example.com"));

                // Assert.That(commit.Committer.TimeOffsetInMinutes, Is.EqualTo(120)); // CEST dependent
                Assert.That(commit.LogMessage, Is.EqualTo("A log message to remember\n"));

                Assert.That(commit.Parents, Is.Not.Empty);
                Assert.That(commit.ParentIds, Is.Not.Empty);

                Assert.That(commit.Tree, Is.Not.Empty);
                Assert.That(commit.Tree.Count, Is.EqualTo(2));
                Assert.That(commit.Ancestor.Tree.Count, Is.EqualTo(3));
                Assert.That(commit.Ancestor.Ancestor.Tree.Count, Is.EqualTo(2));
                Assert.That(commit.Tree.Id, Is.Not.EqualTo(commit.Ancestor.Tree.Id));

                GitId id;
                Assert.That(repo1.LookupViaPrefix(commit.Id.ToString(), out id));
                Assert.That(id, Is.EqualTo(commit.Id));

                Assert.That(repo1.LookupViaPrefix(commit.Id.ToString().Substring(0,10), out id));
                Assert.That(id, Is.EqualTo(commit.Id));

                Assert.That(commit.Peel<GitObject>().Id, Is.EqualTo(commit.Tree.Id));
                Assert.That(commit.Peel<GitTree>(), Is.EqualTo(commit.Tree)); // Compares members
                Assert.That(commit.Tree.Peel<GitObject>(), Is.Null);

                GitTagArgs ta = new GitTagArgs();
                ta.Tagger.When = ct;
                ta.Tagger.Name = "Me";
                ta.Tagger.EmailAddress = "me@myself.and.I";
                ta.LogMessage = "Some message";
                ga.Author.TimeOffsetInMinutes = 120;
                Assert.That(commit.Tag("MyTag", ta, out id));
                Assert.That(id, Is.EqualTo(new GitId("c7baaa514b4c05a4c140fd2623948f0ddf847952")));

                GitTag tag;
                Assert.That(repo1.Lookup(id, out tag));
                Assert.That(tag, Is.Not.Null);
                Assert.That(tag.LogMessage, Is.EqualTo("Some message\n"));
                Assert.That(tag.Tagger.Name, Is.EqualTo("Me"));

                //Console.WriteLine("1:");
                //foreach (GitTreeEntry e in commit.Tree)
                //{
                //    Console.WriteLine(string.Format("{0}: {1} ({2})", e.Name, e.Kind, e.Children.Count));
                //}

                //Console.WriteLine("2:");
                //foreach (GitTreeEntry e in commit.Ancestor.Tree)
                //{
                //    Console.WriteLine(string.Format("{0}: {1} ({2})", e.Name, e.Kind, e.Children.Count));
                //}

                //Console.WriteLine("3:");
                //foreach (GitTreeEntry e in commit.Ancestor.Ancestor.Tree)
                //{
                //    Console.WriteLine(string.Format("{0}: {1} ({2})", e.Name, e.Kind, e.Children.Count));
                //}
                //Console.WriteLine("-");
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
                Assert.That(repo.RepositoryDirectory, Is.EqualTo(Path.Combine(dir, GitClient.AdministrativeDirectoryName)));
                Assert.That(repo.WorkingCopyDirectory, Is.EqualTo(dir));

                GitConfiguration config = repo.Configuration;

                Assert.That(config, Is.Not.Null);

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
