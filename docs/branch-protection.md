# Protecting the main branch

## Policy

The main branch is the stable integration branch. **Direct pushes, force pushes and deletion of main are not allowed. All changes must be merged through pull requests.**

The rules below are intended to enforce this policy on GitHub, not merely describe it.

## Required GitHub settings

Open the repository:

1. Go to Settings.
2. Open Rules → Rulesets (recommended), or Settings → Branches → Add branch protection rule.
3. Target the branch named main.
4. Enable the following rules.

| Setting | Required value | Purpose |
|---|---|---|
| Require a pull request before merging | On | Prevent direct changes from bypassing review |
| Required approvals | 1 | Require another team member to review |
| Dismiss stale approvals | On | A new commit invalidates an old approval |
| Require conversation resolution | On | Do not merge unresolved review comments |
| Require status checks to pass | On | Build must pass before merge |
| Required status check | build-linux | GitHub Actions build job |
| Block force pushes | On | Preserve branch history |
| Block branch deletion | On | Protect the integration branch |
| Bypass list / bypass actors | Empty | Do not allow routine administrator bypass |

If using a classic branch protection rule, enable **Do not allow bypassing the above settings** so administrators are covered too, when this option is available for the repository.

## Recommended merge settings

- Allow squash merge for focused feature pull requests.
- Optionally allow rebase merge if the team understands the resulting history.
- Disable force pushes to main.
- Do not enable auto-merge until required checks and approvals are configured.
- Keep main as the default branch.

## Team process

1. Each contributor creates a branch from latest main.
2. Push commits only to the contributor's branch.
3. Open a pull request targeting main.
4. GitHub Actions runs the build workflow.
5. A teammate other than the author reviews and approves.
6. Resolve all conversations.
7. Merge after the required status check and approval are green.

## Verification

After saving the rules, test with a non-admin collaborator:
- A direct push to main should be rejected.
- A pull request without an approval should not be mergeable.
- A pull request with a failing build should not be mergeable.
- A pull request with one approval and passing CI should be mergeable.

## Current configuration note

The repository contents can document this policy, but branch protection is a repository-level GitHub setting, not a source file. The repository Rulesets API returned an empty list when checked. The classic branch-protection read endpoint returned 403 to the connected GitHub integration, and the available GitHub actions do not expose a write operation for changing repository protection settings. Therefore the current classic branch-protection state could not be verified or changed from this session. A repository owner must apply the settings above in GitHub Settings.

After configuring it, verify the result under Settings → Rules → Rulesets (or Settings → Branches). Do not claim the branch is protected until GitHub shows the rule as active.
