//
//  HomeView.swift
//  Patriot
//
//  Created by Ron Lisle on 5/25/21.
//

import SwiftUI

struct HomeView: View {

    let sideMenuWidth: CGFloat = 200
        
    @EnvironmentObject var model: PatriotModel
    @EnvironmentObject var appDelegate: AppDelegate
    @EnvironmentObject var sceneDelegate: SceneDelegate
    
//    init() {
//        UINavigationBar.appearance().titleTextAttributes = [.foregroundColor: UIColor(.white)]
//        UINavigationBar.appearance().backgroundColor = [.foregroundColor: UIColor(.white)]
//    }
    
    var body: some View {
        
        let dragToClose = DragGesture()
            .onEnded {
                if $0.translation.width < -100 {
                    withAnimation {
                        model.showingMenu = false
                    }
                }
            }
        
        NavigationView {
            ZStack(alignment: .leading) {
                NavigationLink("", isActive: $model.showingDetails, destination: {
                    DeviceDetailView()
                })
                MainView()

                .offset(x: model.showingMenu ? sideMenuWidth : 0)
                .disabled(model.showingMenu ? true : false)

                if model.showingMenu {
                    MenuView()
                        .frame(width: sideMenuWidth)
                }
            }
            .background(Color("BackgroundColor"))
            .gesture(dragToClose)

            .navigationBarTitle("Patriot")
            .navigationBarTitleDisplayMode(.inline)
//            .foregroundColor(.white)
            .toolbar {
                ToolbarItemGroup(placement: .navigationBarLeading) {
                    SideMenuButton(showMenu: $model.showingMenu)
                }
                ToolbarItemGroup(placement: .navigationBarTrailing) {
                    SleepingButton(sleeping: model.sleeping)
                    PartOfDayButton()
                }
            }
            
        }
        
        
        // This fixes the layout constraint warnings
        .navigationViewStyle(StackNavigationViewStyle())
        
        .onAppear {
             let appearance = UINavigationBarAppearance()
//             appearance.backgroundEffect = UIBlurEffect(style: .systemUltraThinMaterial)
             appearance.backgroundColor = UIColor(Color("HeadingBackground"))
            appearance.titleTextAttributes = [.foregroundColor: UIColor(.white)]
             UINavigationBar.appearance().standardAppearance = appearance
             UINavigationBar.appearance().scrollEdgeAppearance = appearance
         }
    }
}

struct HomeView_Previews: PreviewProvider {
    static var previews: some View {
        HomeView()
            .environmentObject(PatriotModel(forTest: true))
    }
}
