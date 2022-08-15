import React from "react";

import {
  Navbar,
  RepoMenu,
  RepoNav,
  FileExplorer,
  Sidebar,
  Footer,
} from "./components";

function App() {
  return (
    <div className="text-gray-900 text-sm">
      <Navbar />
      <RepoNav />
      <RepoMenu />

      <div className="container mx-auto my-4 px-4 flex">
        <FileExplorer />
        <Sidebar />
      </div>

      <Footer />
    </div>
  );
}

export default App;
